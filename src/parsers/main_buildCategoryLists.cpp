#include <iostream>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>
#include <iomanip>
#include <map>
#include <future>
#include <thread>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <fstream>

#include <boost/graph/graphml.hpp>
#include <boost/graph/graphviz.hpp>

#include "wikiHandlers/categoryArticleHandler.h"
#include "xml/wikiDumpHandler.h"
#include "wikiHandlers/tagFilterHandler.h"
#include "wikiHandlers/linkExtractionHandler.h"
#include "wikiHandlers/articleTitlesHandler.h"
#include "wikiHandlers/categoryListHandler.h"
#include "wikiClassDetection/wikiClasses.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char* argv[])
{
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "Produce help message.")
		("input-xml-folder", po::value<std::string>(), "The folder that should be scanned for wikidump .xml files.")
		("output-folder", po::value<std::string>(), "The .graphml file which contains an already extracted graph.")
		("max-threads", po::value<std::size_t>(), "The maximal number of parallel threads that should be used for parsing.")
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
	const fs::path outputFolder(vm["input-xml-folder"].as<std::string>());
	//const bool inputGraphSpecified = vm.count("input-graph-graphml");
	//std::size_t numMaxThreads = vm.count("max-threads") ? vm["max-threads"].as<std::size_t>() : std::thread::hardware_concurrency();

	if(!fs::is_directory(inputFolder))
	{
		std::cerr << "Parameter --input-xml-folder is no folder." << std::endl;
		return 1;
	}

	if(!fs::is_directory(outputFolder))
	{
		std::cerr << "Parameter --ouput-folder is no folder." << std::endl;
		return 1;
	}

	try {
		xercesc::XMLPlatformUtils::Initialize();
	}
	catch (const xercesc::XMLException& toCatch) {
		char* message = xercesc::XMLString::transcode(toCatch.getMessage());
		std::cout << "Error during initialization! :\n";
		std::cout << "Exception message is: \n" << message << "\n";
		xercesc::XMLString::release(&message);
		return 1;
	}

	std::vector<fs::path> xmlFileList;
	for(auto dir_it = fs::directory_iterator(inputFolder); dir_it != fs::directory_iterator(); dir_it++)
	{
		if(!fs::is_directory(dir_it->path()))
			xmlFileList.push_back(dir_it->path());
	}

	ArticleTitlesHandler articleTitlesHandler; 

	for (auto el : xmlFileList) {
		std::cout << el << std::endl;

		xercesc::SAX2XMLReader* parser = xercesc::XMLReaderFactory::createXMLReader();
		parser->setFeature(xercesc::XMLUni::fgSAX2CoreValidation, true);
		parser->setFeature(xercesc::XMLUni::fgSAX2CoreNameSpaces, true);   // optional
		parser->setFeature(xercesc::XMLUni::fgXercesSchema , false);   // optional
		
		WikiDumpHandler handler(articleTitlesHandler, false);
		parser->setContentHandler(&handler);
		parser->setErrorHandler(&handler);
		
		try {
			parser->parse(el.c_str());
			delete parser;
		}
		catch (const xercesc::XMLException& toCatch) {
			char* message = xercesc::XMLString::transcode(toCatch.getMessage());
			std::cerr << "Exception message is: \n" << message << "\n";
			xercesc::XMLString::release(&message);
			throw std::logic_error("An error occurred");
		}
		catch (const xercesc::SAXParseException& toCatch) {
			char* message = xercesc::XMLString::transcode(toCatch.getMessage());
			std::cerr << "Exception message is: \n" << message << "\n";
			xercesc::XMLString::release(&message);
			throw std::logic_error("An error occurred");
		}
	}

	std::cout << "Num Titles: " << articleTitlesHandler.articles.size() << std::endl;

	std::vector<std::string> articles(articleTitlesHandler.articles.begin(), articleTitlesHandler.articles.end());

	CategoryListHandler categoryListHandler(articles); 

	for (auto el : xmlFileList) {
		std::cout << el << std::endl;

		xercesc::SAX2XMLReader* parser = xercesc::XMLReaderFactory::createXMLReader();
		parser->setFeature(xercesc::XMLUni::fgSAX2CoreValidation, true);
		parser->setFeature(xercesc::XMLUni::fgSAX2CoreNameSpaces, true);   // optional
		parser->setFeature(xercesc::XMLUni::fgXercesSchema , false);   // optional
		
		WikiDumpHandler handler(categoryListHandler, true);
		parser->setContentHandler(&handler);
		parser->setErrorHandler(&handler);
		
		try {
			parser->parse(el.c_str());
			delete parser;
		}
		catch (const xercesc::XMLException& toCatch) {
			char* message = xercesc::XMLString::transcode(toCatch.getMessage());
			std::cerr << "Exception message is: \n" << message << "\n";
			xercesc::XMLString::release(&message);
			throw std::logic_error("An error occurred");
		}
		catch (const xercesc::SAXParseException& toCatch) {
			char* message = xercesc::XMLString::transcode(toCatch.getMessage());
			std::cerr << "Exception message is: \n" << message << "\n";
			xercesc::XMLString::release(&message);
			throw std::logic_error("An error occurred");
		}
	}

	for (auto categoryObj : categoryListHandler.categoryChildren) {
		std::cout << articles[categoryObj.first] << ": ";
		for (auto childId : categoryObj.second) {
			std::cout << articles[childId] << " | ";
		}	
		std::cout << std::endl;
	}

	try {
		xercesc::XMLPlatformUtils::Terminate();
	}
	catch (const xercesc::XMLException& toCatch) {
		char* message = xercesc::XMLString::transcode(toCatch.getMessage());
		std::cout << "Error during termination! :\n";
		std::cout << "Exception message is: \n" << message << "\n";
		xercesc::XMLString::release(&message);
		return 1;
	}

	return 0;
}
