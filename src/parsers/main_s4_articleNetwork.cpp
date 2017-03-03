// stdlib
#include <iostream>
#include <iomanip>
#include <map>
#include <future>
#include <thread>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <fstream>

// xerces
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>

// boost
#include <boost/algorithm/string/split.hpp>
#include <boost/container/flat_set.hpp>

// local files
#include "../core/date.h"
#include "shared.h"
#include "../core/wikiDataCache.h"


#include "wikiArticleHandlers/linkExtractionHandler.h"
#include "../../libs/wiki_xml_dump_xerces/src/parsers/parallelParser.hpp"
#include "../../libs/wiki_xml_dump_xerces/src/handlers/wikiDumpHandlerProperties.hpp"



namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace WikiMainPath;

int main(int argc, char* argv[])
{
	// setup timings stuff
	auto globalStartTime = std::chrono::steady_clock::now();
	std::vector<std::pair<std::string, double>> timings;

	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "Produce help message.")
		("input-xml-folder", po::value<std::string>(), "The folder that should be scanned for wikidump .xml files.")
		("input-article-folder", po::value<std::string>(), "The folder that should contain articlesWithDates.txt, categories.txt, redirects.txt files.")
		("output-folder", po::value<std::string>(), "The folder in which the results (articlesWithDates.txt, categories.txt, redirects.txt) should be stored.")
		("page-counts-file", po::value<std::string>(), "The file that contains counts of pages for each .xml file.")
		;
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	// display help if --help was specified
	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return 0;
	}

	if(!vm.count("input-article-folder") || !vm.count("input-xml-folder") || !(vm.count("output-graph") ))
	{
		std::cerr << "Please specify the parameters --input-article-folder, --input-xml-folder and --output-graph-graphml or --output-graph-minimized." << std::endl;
		return 1;
	}

	const fs::path inputXmlFolder(vm["input-xml-folder"].as<std::string>());
	const fs::path inputArticleFolder(vm["input-article-folder"].as<std::string>());
	const fs::path outputFolder(vm["output-folder"].as<std::string>());

	auto pageCounts = (vm.count("page-counts-file") ? readPageCountsFile(vm["page-counts-file"].as<std::string>()) : std::map<std::string, std::size_t>());

	if(!fs::is_directory(inputXmlFolder))
	{
		std::cerr << "Parameter --input-xml-folder is no folder." << std::endl;
		return 1;
	}

	if(!fs::is_directory(inputArticleFolder))
	{
		std::cerr << "Parameter --input-article-folder is no folder." << std::endl;
		return 1;
	}

	if(!fs::exists(inputArticleFolder / "articles_with_dates.txt") || !fs::exists(inputArticleFolder / "categories.txt") || !fs::exists(inputArticleFolder / "redirects.txt"))
	{
		std::cerr << "The input article folder does not contain all necessary files." << std::endl;
		return 1;
	}

	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Reading in already parsed files... " << std::endl;
	auto startTime = std::chrono::steady_clock::now();

	WikiDataCache wiki_data_cache(outputFolder.string());
	const auto& articles = wiki_data_cache.article_titles();
	const auto& dates = wiki_data_cache.article_dates();
	const auto& redirects = wiki_data_cache.redirects();

	auto endTime = std::chrono::steady_clock::now();
	auto diff = std::chrono::duration<double, std::milli>(endTime-startTime).count();
	timings.push_back({ "Reading in already parsed files...", diff });

	// initialize xerces
	xercesc::XMLPlatformUtils::Initialize();

	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Found the following .xml files: " << std::endl;
	// scan xml folder for files and put them into list
	std::vector<fs::path> xmlFileList;
	for(auto dir_it = fs::directory_iterator(inputXmlFolder); dir_it != fs::directory_iterator(); dir_it++)
	{
		if(!fs::is_directory(dir_it->path()))
			xmlFileList.push_back(dir_it->path());
	}

	std::vector<std::string> paths;
	for (auto el : xmlFileList) {
		std::cout << el << std::endl;
		paths.push_back(el.string());
	}

	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Parsing .xml files" << std::endl;
	startTime = std::chrono::steady_clock::now();

	VectorMutex<1000> vecMutex;
	std::vector<boost::container::flat_set<std::size_t>> adjList(articles.size());

	// init xerces
	xercesc::XMLPlatformUtils::Initialize();

	WikiXmlDumpXerces::WikiDumpHandlerProperties parser_properties;
	parser_properties.TitleFilter = [](const std::string& title) {
		return !(
				title.substr(0,5) == "User:" 
				|| title.substr(0,10) == "Wikipedia:" 
				|| title.substr(0,5) == "File:" 
				|| title.substr(0,14) == "Category talk:" 
				|| title.substr(0,9) == "Category:"
				|| title.substr(0,14) == "Template talk:"
				|| title.substr(0,9) == "Template:"
				|| title.substr(0,10) == "User talk:"
				|| title.substr(0,10) == "File talk:"
				|| title.substr(0,15) == "Wikipedia talk:"
				);
	};
		
	parser_properties.ProgressCallback = std::bind(printProgress, pageCounts, std::placeholders::_2, std::placeholders::_1, std::placeholders::_3);

	WikiXmlDumpXerces::ParallelParser<LinkExtractionHandler> parser([&articles, &redirects, &adjList, &vecMutex, &dates](){ 
		auto handler = LinkExtractionHandler(articles, redirects, adjList, vecMutex); 

		handler.OrderCallback = [&dates] (std::size_t art1, std::size_t art2)
		{
			return dates[art1] < dates[art2];
		};

		return handler;
	}, parser_properties);
	parser.Run(paths.begin(), paths.end());

	// terminate xerces
	xercesc::XMLPlatformUtils::Terminate();

	// std::vector<LinkExtractionHandler> handlers;
	// std::vector<std::future<void>> futures;
	// for(std::size_t i = 0; i < xmlFileList.size(); i++)
	// {
		// auto xmlPath = xmlFileList[i];
		// //handlers.emplace_back(LinkExtractionHandler(articles, dates, redirects, adjList, vecMutex)); 
		// S4ParserWrapper wrapper(xmlPath, pageCounts, articles, dates, redirects, adjList, vecMutex);
		// wrapper.OrderCallback = [&dates] (std::size_t art1, std::size_t art2)
		// {
			// return dates[art1] < dates[art2];
		// };
		// futures.emplace_back(std::async(std::launch::async, wrapper));
	// }

	// for (auto& fut : futures) 
		// fut.get();	

	endTime = std::chrono::steady_clock::now();
	diff = std::chrono::duration<double, std::milli>(endTime-startTime).count();
	timings.push_back({ "Parsing .xml files.", diff });
	

	startTime = std::chrono::steady_clock::now();

	wiki_data_cache.write_article_network(adjList);

	endTime = std::chrono::steady_clock::now();
	diff = std::chrono::duration<double, std::milli>(endTime-startTime).count();
	timings.push_back({ "Writing output files", diff });

	xercesc::XMLPlatformUtils::Terminate();

	endTime = std::chrono::steady_clock::now();
	diff = std::chrono::duration<double, std::milli>(endTime-globalStartTime).count();
	timings.push_back({ "Total", diff });
	
	// output timings
	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Timings: " << std::endl << std::endl;
	for(auto timing : timings)
		std::cout << std::left << std::setw(50) << timing.first + ": " << timingToReadable(timing.second) << std::endl;

	std::cout << "-----------------------------------------------------------------------" << std::endl;



	return 0;
}
