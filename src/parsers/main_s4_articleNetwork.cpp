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
#include "wikiHandlers/linkExtractionHandler.h"
#include "xml/wikiDumpHandler.h"
#include "articleNetwork/dateExtractor.h"
#include "shared.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

void readDataFromFile (const fs::path& inputFolder, std::vector<std::string>& articles, std::vector<Date>& dates, std::map<std::string, std::string>& redirects)
{
	std::ifstream articles_file((inputFolder / "articles_with_dates.txt").string());	
	std::ifstream redirects_file((inputFolder / "redirects.txt").string());	
		
	std::string line;
	while(std::getline(articles_file, line))
	{
		std::istringstream ss(line);
		std::string title, dateStr;
		std::getline(ss, title, '\t');
		std::getline(ss, dateStr, '\t');
		articles.push_back(title);
		dates.push_back(Date::deserialize(dateStr));
	}			

	while(std::getline(redirects_file, line))
	{
		std::istringstream ss(line);
		std::string sourceTitle, targetTitle;
		std::getline(ss, sourceTitle, '\t');
		std::getline(ss, targetTitle, '\t');
		redirects.insert({ sourceTitle, targetTitle });
	}
}


class ParserWrapper 
{
	public:
		ParserWrapper(
			fs::path path, 
			const std::map<std::string, std::size_t>& pageCounts, 
			const std::vector<std::string>& arts, 
			const std::vector<Date>& dates, 
			const std::map<std::string, std::string>& redirs,
			std::vector<boost::container::flat_set<std::size_t>>& adjList,
			VectorMutex<1000>& vecMut	
		)
		:_path(path),
		_pageCounts(pageCounts),
		_articles(arts),
		_dates(dates),
		_redirects(redirs),
		_adjList(adjList),
		_vecMutex(vecMut)
		{}

		void operator()(void)
		{
			LinkExtractionHandler artHandler(_articles, _dates, _redirects, _adjList, _vecMutex); 
			xercesc::SAX2XMLReader* parser = xercesc::XMLReaderFactory::createXMLReader();
			parser->setFeature(xercesc::XMLUni::fgSAX2CoreValidation, true);
			parser->setFeature(xercesc::XMLUni::fgSAX2CoreNameSpaces, true);   // optional
			parser->setFeature(xercesc::XMLUni::fgXercesSchema , false);   // optional

			WikiDumpHandler handler(artHandler, true);
			handler.TitleFilter = [](const std::string& title) {
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
			handler.ProgressCallback = std::bind(printProgress, _pageCounts, _path, std::placeholders::_1);

			parser->setContentHandler(&handler);
			parser->setErrorHandler(&handler);

			parser->parse(_path.c_str());
			delete parser;
		}

	private:
		fs::path _path;
		const std::map<std::string, std::size_t>& _pageCounts;

		const std::vector<std::string>& _articles;
		const std::vector<Date>& _dates; 
		const std::map<std::string, std::string>& _redirects;
		std::vector<boost::container::flat_set<std::size_t>>& _adjList;
		VectorMutex<1000>& _vecMutex;
};


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
		("output-graph", po::value<std::string>(), "The graph file which should contain the output graph.")
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

	std::vector<std::string> articles;
	std::vector<Date> dates;
	std::map<std::string, std::string> redirects;

	readDataFromFile(inputArticleFolder, articles, dates, redirects);

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
		std::cout << dir_it->path() << std::endl;
	}

	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Parsing .xml files" << std::endl;
	startTime = std::chrono::steady_clock::now();
	std::vector<LinkExtractionHandler> handlers;
	std::vector<std::future<void>> futures;
	VectorMutex<1000> vecMutex;
	std::vector<boost::container::flat_set<std::size_t>> adjList(articles.size());
	for(std::size_t i = 0; i < xmlFileList.size(); i++)
	{
		auto xmlPath = xmlFileList[i];
		//handlers.emplace_back(LinkExtractionHandler(articles, dates, redirects, adjList, vecMutex)); 
		futures.emplace_back(std::async(std::launch::async, ParserWrapper(xmlPath, pageCounts, articles, dates, redirects, adjList, vecMutex)));
	}

	for (auto& fut : futures) 
		fut.get();	

	endTime = std::chrono::steady_clock::now();
	diff = std::chrono::duration<double, std::milli>(endTime-startTime).count();
	timings.push_back({ "Parsing .xml files.", diff });
	

	startTime = std::chrono::steady_clock::now();

	std::ofstream graphFile(vm["output-graph"].as<std::string>());
	for (auto arts : adjList) 
	{
		for (auto art : arts) 
			graphFile << art << " ";
			
		graphFile << std::endl;
	}

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
