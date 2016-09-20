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

// local files
#include "wikiHandlers/linkExtractionHandler.h"
#include "xml/wikiDumpHandler.h"
#include "articleNetwork/articleGraphIo.h"
#include "articleNetwork/dateExtractor.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

UndirectedArticleGraph createGraphFromParameters(po::variables_map& vm)
{
	if(vm.count("input-graph-minimized"))
	{
		std::ifstream inFile(vm["input-graph-minimized"].as<std::string>());
		return readMinimizedGraph(inFile);
	}

	if(vm.count("input-graph-graphml"))
	{
		std::ifstream inFile(vm["input-graph-graphml"].as<std::string>());
		return readFromGraphMl(inFile);
	}

	return UndirectedArticleGraph();
}

void readDataFromFile (const fs::path& inputFolder, std::vector<std::string>& articles, std::vector<std::tm>& dates, std::map<std::string, std::vector<std::string>>& categoriesToArticles, std::map<std::string, std::string>& redirects)
{
	std::ifstream articles_file((inputFolder / "articles_with_dates.txt").string());	
	std::ifstream categories_file((inputFolder / "categories.txt").string());	
	std::ifstream redirects_file((inputFolder / "redirects.txt").string());	
		
	std::string line;
	while(std::getline(articles_file, line))
	{
		std::istringstream ss(line);
		std::string title, dateStr;
		std::getline(ss, title, '\t');
		std::getline(ss, dateStr, '\t');
		articles.push_back(title);
		dates.push_back(DateExtractor::deserialize(dateStr));
	}			

	while(std::getline(categories_file, line))
	{
		std::istringstream ss(line);
		std::string categoryTitle, articlesStr;
		std::getline(ss, categoryTitle, '\t');
		std::getline(ss, articlesStr, '\t');
		std::vector<std::string> articlesVec;
		boost::split(articlesVec, articlesStr, ";-;");
		std::cout << categoryTitle << "    -->   ";
		for (auto i : articlesVec) {
			std::cout << i << " --------- ";	
		}
		std::cout << std::endl;
		categoriesToArticles.insert({ categoryTitle, articlesVec });
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

int main(int argc, char* argv[])
{
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "Produce help message.")
		("input-xml-folder", po::value<std::string>(), "The folder that should be scanned for wikidump .xml files.")
		("input-article-folder", po::value<std::string>(), "The folder that should contain articlesWithDates.txt, categories.txt, redirects.txt files.")
		("input-graph-graphml", po::value<std::string>(), "The .graphml file which contains an already extracted graph.")
		("input-graph-minimized", po::value<std::string>(), "The graph file which contains an already extracted graph.")
		("output-graph-graphml", po::value<std::string>(), "The .graphml file which should contain the output graph.")
		("output-graph-minimized", po::value<std::string>(), "The graph file which should contain the output graph.")
	;
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	// display help if --help was specified
	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return 0;
	}

	if(!vm.count("input-article-folder") || !vm.count("input-xml-folder") || !(vm.count("output-graph-graphml") || vm.count("output-graph-minimized")))
	{
		std::cerr << "Please specify the parameters --input-article-folder, --input-xml-folder and --output-graph-graphml or --output-graph-minimized." << std::endl;
		return 1;
	}

	const fs::path inputXmlFolder(vm["input-xml-folder"].as<std::string>());
	const fs::path inputArticleFolder(vm["input-article-folder"].as<std::string>());

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

	std::vector<std::string> articles;
	std::vector<std::tm> dates;
   	std::map<std::string, std::vector<std::string>> categoriesToArticles;
   	std::map<std::string, std::string> redirects;

	readDataFromFile(inputArticleFolder, articles, dates, categoriesToArticles, redirects);

	// initialize xerces
	xercesc::XMLPlatformUtils::Initialize();

	// scan xml folder for files and put them into list
	std::vector<fs::path> xmlFileList;
	for(auto dir_it = fs::directory_iterator(inputXmlFolder); dir_it != fs::directory_iterator(); dir_it++)
	{
		if(!fs::is_directory(dir_it->path()))
			xmlFileList.push_back(dir_it->path());
	}

	//UndirectedArticleGraph g = createGraphFromParameters(vm);
	DirectedArticleGraph g = DirectedArticleGraph(articles.size());
	LinkExtractionHandler linkExtractionHandler(g, articles, dates, categoriesToArticles, redirects); 

	for (auto el : xmlFileList) {
		std::cout << el << std::endl;

		xercesc::SAX2XMLReader* parser = xercesc::XMLReaderFactory::createXMLReader();
		parser->setFeature(xercesc::XMLUni::fgSAX2CoreValidation, true);
		parser->setFeature(xercesc::XMLUni::fgSAX2CoreNameSpaces, true);   // optional
		parser->setFeature(xercesc::XMLUni::fgXercesSchema , false);   // optional
		
		WikiDumpHandler handler(linkExtractionHandler, true);
		handler.TitleFilter = [](const std::string& title) {
			return !(title.substr(0,5) == "User:" || title.substr(0,10) == "Wikipedia:" || title.substr(0,5) == "File:" || title.substr(0,5) == "Talk:" || title.substr(0,9) == "Category:");
		};
		parser->setContentHandler(&handler);
		parser->setErrorHandler(&handler);
		
		parser->parse(el.c_str());
		delete parser;
	}

	std::cout << "Number of nodes: " << boost::num_vertices(linkExtractionHandler.graph()) << std::endl;
	std::cout << "Number of edges: " << boost::num_edges(linkExtractionHandler.graph()) << std::endl;

	drawToSvg("hallo.svg", linkExtractionHandler.graph(), DOT);

	if(vm.count("output-graph-minimized"))
	{
		std::ofstream graphFile(vm["output-graph-minimized"].as<std::string>());
		writeMinimizedGraph(graphFile, linkExtractionHandler.graph());
	}

	if(vm.count("output-graph-graphml"))
	{
		std::ofstream graphFile(vm["output-graph-graphml"].as<std::string>());
		writeToGraphMl(graphFile, linkExtractionHandler.graph());
	}

	xercesc::XMLPlatformUtils::Terminate();

	return 0;
}
