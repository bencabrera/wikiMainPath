// stdlib
#include <iostream>
#include <iomanip>
#include <map>
#include <future>
#include <thread>
#include <fstream>

// xerces
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>

// boost
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

// local files
#include "xml/wikiDumpHandler.h"
#include "wikiHandlers/articleTitlesHandler.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char* argv[])
{
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "Produce help message.")
		("input-xml-folder", po::value<std::string>(), "The folder that should be scanned for wikidump .xml files.")
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

	// init xerces
	xercesc::XMLPlatformUtils::Initialize();

	// scan for xml files in the input-folder
	std::vector<fs::path> xmlFileList;
	for(auto dir_it = fs::directory_iterator(inputFolder); dir_it != fs::directory_iterator(); dir_it++)
	{
		if(!fs::is_directory(dir_it->path()))
			xmlFileList.push_back(dir_it->path());
	}

	// setup and run the handler for running over all entrys in xml file and extracting titles and dates
	ArticleTitlesHandler articleTitlesHandler; 

	for (auto el : xmlFileList) 
	{
		std::cout << el << std::endl;

		xercesc::SAX2XMLReader* parser = xercesc::XMLReaderFactory::createXMLReader();
		parser->setFeature(xercesc::XMLUni::fgSAX2CoreValidation, true);
		parser->setFeature(xercesc::XMLUni::fgSAX2CoreNameSpaces, true);   // optional
		parser->setFeature(xercesc::XMLUni::fgXercesSchema , false);   // optional
		
		WikiDumpHandler handler(articleTitlesHandler, true);
		handler.TitleFilter = [](const std::string& title) {
			return !(title.substr(0,5) == "User:" || title.substr(0,10) == "Wikipedia:" || title.substr(0,5) == "File:" || title.substr(0,5) == "Talk:" || title.substr(0,9) == "Category:");
		};
		parser->setContentHandler(&handler);
		parser->setErrorHandler(&handler);
		
		parser->parse(el.c_str());
		delete parser;
	}

	std::ofstream articles_file((outputFolder / "articles_with_dates.txt").string());	
	for(auto article : articleTitlesHandler.articles)
		articles_file << article.first << "\t" << article.second.tm_year << "-" << article.second.tm_mon << "-" << article.second.tm_mday << std::endl;

	std::ofstream categories_file((outputFolder / "categories.txt").string());	
	for(auto category : articleTitlesHandler.categoriesToArticles)
	{
		categories_file << category.first << "\t";
		for (auto article : category.second) {
			categories_file << article << ";";	
		}
		categories_file << std::endl;
	}

	std::ofstream redirects_file((outputFolder / "redirects.txt").string());	
	for (auto redirect : articleTitlesHandler.redirects) 
		redirects_file << redirect.first << ";" << redirect.second << std::endl;	
	
	// short feedback to user
	std::cout << "Found ... articles of which ... had extractable dates. Also found ... categories." << std::endl;

	xercesc::XMLPlatformUtils::Terminate();

	return 0;
}
