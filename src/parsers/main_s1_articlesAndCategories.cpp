// stdlib
#include <iostream>
#include <iomanip>
#include <map>
#include <future>
#include <thread>
#include <fstream>
#include <functional>

// xerces
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>

// boost
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

// local files
#include "shared.h"
// #include "parserWrappers/s1_wrapper.h"
#include "fileNames.h"

// wiki xml dump lib
#include "wikiArticleHandlers/articleDatesAndCategoriesHandler.h"
#include "../../libs/wiki_xml_dump_xerces/src/parsers/parallelParser.hpp"
#include "../../libs/wiki_xml_dump_xerces/src/handlers/wikiDumpHandlerProperties.hpp"

namespace po = boost::program_options;
namespace fs = boost::filesystem;


int main(int argc, char* argv[])
{
	using namespace WikiMainPath;

	// setup timings stuff
	auto globalStartTime = std::chrono::steady_clock::now();
	std::vector<std::pair<std::string, double>> timings;

	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "Produce help message.")
		("input-xml-folder", po::value<std::string>(), "The folder that should be scanned for wikidump .xml files.")
		("page-counts-file", po::value<std::string>(), "The file that contains counts of pages for each .xml file.")
		("output-folder", po::value<std::string>(), "The folder in which the results (articlesWithDates.txt, categories.txt, redirects.txt) should be stored.")
		;
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	// display help if --help was specified
	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return 0;
	}

	if(!vm.count("input-xml-folder") || !vm.count("output-folder"))
	{
		std::cerr << "Please specify the parameters --input-xml-folder and --output-folder." << std::endl;
		return 1;
	}

	const fs::path inputFolder(vm["input-xml-folder"].as<std::string>());
	const fs::path outputFolder(vm["output-folder"].as<std::string>());

	auto pageCounts = (vm.count("page-counts-file") ? readPageCountsFile(vm["page-counts-file"].as<std::string>()) : std::map<std::string, std::size_t>());

	if(!fs::is_directory(inputFolder))
	{
		std::cerr << "Parameter --input-xml-folder is no folder." << std::endl;
		return 1;
	}

	if(!fs::is_directory(outputFolder))
	{
		std::cerr << "Parameter --ouput-folder is no folder, creating it..." << std::endl;
		fs::create_directory(outputFolder);
	}


	// scan for xml files in the input-folder
	std::vector<fs::path> xmlFileList;
	for(auto dir_it = fs::directory_iterator(inputFolder); dir_it != fs::directory_iterator(); dir_it++)
	{
		if(!fs::is_directory(dir_it->path()))
			xmlFileList.push_back(dir_it->path());
	}

	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Found the following files: " << std::endl;
	std::vector<std::string> paths;
	for (auto el : xmlFileList) {
		std::cout << el << std::endl;
		paths.push_back(el.string());
	}


	// setup and run the handler for running over all entrys in xml file and extracting titles and dates

	auto startTime = std::chrono::steady_clock::now();

	// init xerces
	xercesc::XMLPlatformUtils::Initialize();

	WikiXmlDumpXerces::WikiDumpHandlerProperties parser_properties;
	parser_properties.TitleFilter = [](const std::string& title) {
		return !(
				title.substr(0,5) == "User:" 
				|| title.substr(0,10) == "Wikipedia:" 
				|| title.substr(0,5) == "File:" 
				|| title.substr(0,14) == "Category talk:" 
				|| title.substr(0,14) == "Template talk:"
				|| title.substr(0,9) == "Template:"
				|| title.substr(0,10) == "User talk:"
				|| title.substr(0,10) == "File talk:"
				|| title.substr(0,15) == "Wikipedia talk:"
				);
	};
	parser_properties.ProgressCallback = std::bind(printProgress, pageCounts, "bla", std::placeholders::_1);

	WikiXmlDumpXerces::ParallelParser<ArticleDatesAndCategoriesHandler> parser([](){ return ArticleDatesAndCategoriesHandler(); }, parser_properties);
	parser.Run(paths.begin(), paths.end());

	// terminate xerces
	xercesc::XMLPlatformUtils::Terminate();

	std::map<std::string,Date> articlesWithDates;
	std::vector<std::string> categories;
	std::map<std::string,std::string> redirects;

	for (auto& hand : parser.PageHandlers())
	{
		articlesWithDates.insert(hand.articles.begin(), hand.articles.end());
		categories.insert(categories.end(), hand.categories.begin(), hand.categories.end());
		redirects.insert(hand.redirects.begin(), hand.redirects.end());
	}

	auto endTime = std::chrono::steady_clock::now();
	auto diff = std::chrono::duration<double, std::milli>(endTime-startTime).count();
	timings.push_back({ "Parsing .xml files and merging data structures.", diff });


	startTime = std::chrono::steady_clock::now();
	
	std::sort(categories.begin(), categories.end());

	endTime = std::chrono::steady_clock::now();
	diff = std::chrono::duration<double, std::milli>(endTime-startTime).count();
	timings.push_back({ "Sorting categories vector.", diff });

	startTime = std::chrono::steady_clock::now();
	std::ofstream articles_file((outputFolder / ARTICLES_FILE).string());	
	std::ofstream dates_file((outputFolder / ARTICLE_DATES_FILE).string());	
	for(auto article : articlesWithDates)
	{
		articles_file << article.first << std::endl;
		dates_file << Date::serialize(article.second) << std::endl;
	}

	std::ofstream categories_file((outputFolder / CATEGORIES_FILE).string());	
	for(auto cat : categories)
		categories_file << cat << std::endl;

	std::ofstream redirects_file((outputFolder / REDIRECTS_FILE).string());	
	for (auto redirect : redirects) 
		redirects_file << redirect.first << "\t" << redirect.second << std::endl;	

	endTime = std::chrono::steady_clock::now();
	diff = std::chrono::duration<double, std::milli>(endTime-startTime).count();
	timings.push_back({ "Writing output files", diff });


	endTime = std::chrono::steady_clock::now();
	diff = std::chrono::duration<double, std::milli>(endTime-globalStartTime).count();
	timings.push_back({ "Total", diff });
	
	// short feedback to user
	std::size_t totalArticleNumber = 0;
	for (auto path : xmlFileList) {
		auto it = pageCounts.find(path.filename().c_str());
		if(it != pageCounts.end())
			totalArticleNumber += it->second;
	}
	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Status: " << std::endl;
	std::cout << std::left << std::setw(40) << "Total number of articles: " << totalArticleNumber << std::endl;
	std::cout << std::left << std::setw(40) << "Articles with extractable dates: " << articlesWithDates.size() << std::endl;
	std::cout << std::left << std::setw(40) << "Categories extracted: " << categories.size() << std::endl;
	std::cout << std::left << std::setw(40) << "Redirects extracted: " << redirects.size() << std::endl;
	
	// output timings
	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Timings: " << std::endl << std::endl;
	for(auto timing : timings)
		std::cout << std::left << std::setw(50) << timing.first + ": " << timingToReadable(timing.second) << std::endl;

	std::cout << "-----------------------------------------------------------------------" << std::endl;



	return 0;
}
