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
#include <boost/container/flat_set.hpp>
#include <boost/algorithm/string/trim.hpp>

// local files
#include "xml/wikiDumpHandler.h"
#include "wikiHandlers/categoryHasArticleHandler.h"
#include "shared.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

class ParserWrapper 
{
	public:
		ParserWrapper(
			fs::path path, 
			const std::map<std::string, std::size_t>& pageCounts, 
			const std::vector<std::string>& arts, 
			const std::vector<std::string>& cats, 
			std::vector<boost::container::flat_set<std::size_t>>& catHasArt, 
			VectorMutex<1000>& vecMut
		)
		:_path(path),
		_pageCounts(pageCounts),
		_articles(arts),
		_categories(cats),
		_categoryHasArticle(catHasArt),
		_vecMutex(vecMut)
		{}

		void operator()(void)
		{
			CategoryHasArticleHandler artHandler(_articles, _categories, _categoryHasArticle, _vecMutex);
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

			parser->parse(this->_path.c_str());
			delete parser;
		}

	private:
		fs::path _path;
		const std::map<std::string, std::size_t>& _pageCounts;

		const std::vector<std::string>& _articles;
		const std::vector<std::string>& _categories; 
		std::vector<boost::container::flat_set<std::size_t>>& _categoryHasArticle; 
		VectorMutex<1000>& _vecMutex;
};

void readDataFromFile(const fs::path& inputFolder, std::vector<std::string>& articles, std::vector<std::string>& categories)
{
	std::ifstream articles_file((inputFolder / "articles_with_dates.txt").string());	
	std::ifstream categories_file((inputFolder / "categories.txt").string());	
		
	std::string line;
	while(std::getline(articles_file, line))
	{
		std::istringstream ss(line);
		std::string title, dateStr;
		std::getline(ss, title, '\t');
		std::getline(ss, dateStr, '\t');
		articles.push_back(title);
	}			

	while(std::getline(categories_file, line))
	{
		boost::trim(line);
		categories.push_back(line);
	}
}

int main(int argc, char* argv[])
{
	// setup timings stuff
	auto globalStartTime = std::chrono::steady_clock::now();
	std::vector<std::pair<std::string, double>> timings;

	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "Produce help message.")
		("input-xml-folder", po::value<std::string>(), "The folder that should be scanned for wikidump .xml files.")
		("page-counts-file", po::value<std::string>(), "The file that contains counts of pages for each .xml file.")
		("output-folder", po::value<std::string>(), "The folder in which the results (articlesWithDates.txt, categories.txt, redirects.txt) should be stored.")
		("input-article-folder", po::value<std::string>(), "The folder that should contain articlesWithDates.txt, categories.txt, redirects.txt files.")
		;
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	// display help if --help was specified
	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return 0;
	}

	if(!vm.count("input-xml-folder") || !vm.count("input-article-folder") || !vm.count("output-folder"))
	{
		std::cerr << "Please specify the parameters --input-xml-folder and --input-article-folder and --output-folder." << std::endl;
		return 1;
	}

	const fs::path inputXmlFolder(vm["input-xml-folder"].as<std::string>());
	const fs::path inputArticleFolder(vm["input-article-folder"].as<std::string>());
	const fs::path outputFolder(vm["output-folder"].as<std::string>());

	auto pageCounts = (vm.count("page-counts-file") ? readPageCountsFile(vm["page-counts-file"].as<std::string>()) : std::map<std::string, std::size_t>());

	if(!fs::is_directory(inputXmlFolder) && !fs::is_directory(inputArticleFolder))
	{
		std::cerr << "Parameter --input-xml-folder or --input-article-folder is no folder." << std::endl;
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
	for(auto dir_it = fs::directory_iterator(inputXmlFolder); dir_it != fs::directory_iterator(); dir_it++)
	{
		if(!fs::is_directory(dir_it->path()))
			xmlFileList.push_back(dir_it->path());
	}

	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Reading in already parsed files... " << std::endl;
	auto startTime = std::chrono::steady_clock::now();
	
	std::vector<std::string> articles;
	std::vector<std::string> categories;
	readDataFromFile(inputArticleFolder, articles, categories);	

	auto endTime = std::chrono::steady_clock::now();
	auto diff = std::chrono::duration<double, std::milli>(endTime-startTime).count();
	timings.push_back({ "Reading in already parsed files...", diff });



	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Found the following .xml files: " << std::endl;
	for (auto el : xmlFileList) {
		std::cout << el << std::endl;
	}

	// setup and run the handler for running over all entrys in xml file and extracting titles and dates

	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Parsing .xml files" << std::endl;
	startTime = std::chrono::steady_clock::now();

	std::vector<CategoryHasArticleHandler> handlers;
	std::vector<boost::container::flat_set<std::size_t>> categoryHasArticle(categories.size());
	VectorMutex<1000> vecMutex;
	std::vector<std::future<void>> futures;
	futures.reserve(xmlFileList.size());
	for (auto xmlPath : xmlFileList) 
	{
		//ParserWrapper wrap(xmlPath,pageCounts,handlers.back());
		//wrap();
		futures.emplace_back(std::async(std::launch::async, ParserWrapper(xmlPath,pageCounts,articles,categories,categoryHasArticle,vecMutex)));
	}

	for (auto& future : futures)
		future.get();

	endTime = std::chrono::steady_clock::now();
	diff = std::chrono::duration<double, std::milli>(endTime-startTime).count();
	timings.push_back({ "Parsing .xml files.", diff });




	startTime = std::chrono::steady_clock::now();
	std::ofstream catArtFile((outputFolder / "category_has_article.txt").string());	
	for(std::size_t i = 0; i < categoryHasArticle.size(); i++)
	{
		for (auto art : categoryHasArticle[i]) 
			catArtFile << art << " ";	
		catArtFile << std::endl;
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
