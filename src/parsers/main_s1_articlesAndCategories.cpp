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
#include "wikiHandlers/articleDatesAndCategoriesHandler.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

class ParserWrapper 
{
	public:
		ParserWrapper(fs::path path, const std::map<std::string, std::size_t>& pageCounts)
		:_path(path),
		_pageCounts(pageCounts)
		{}

		ArticleDatesAndCategoriesHandler operator()(void)
		{
			ArticleDatesAndCategoriesHandler artHandler; 

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
			handler.ProgressCallback = [this](std::size_t count)
			{
				auto it = this->_pageCounts.find(this->_path.filename().c_str());
				if(it != this->_pageCounts.end())
					std::cout << this->_path.filename() << ": " << std::right << count << " / " << it->second << "  [" << ((int)(100*(double)count/it->second)) << " %]" << std::endl;
				else
					std::cout << this->_path.filename() << ": " << std::right << count << std::endl;
				std::cout.flush();
			};

			parser->setContentHandler(&handler);
			parser->setErrorHandler(&handler);

			parser->parse(this->_path.c_str());
			delete parser;

			return artHandler;
		}

	private:
		fs::path _path;

		const std::map<std::string, std::size_t>& _pageCounts;
};

std::string timingToReadable(std::size_t milliseconds)
{
	std::stringstream ss;
	
	auto hours = (milliseconds / (1000*60*60));
	auto mins =  (milliseconds % (1000*60*60)) / (1000*60);
	auto seconds =  ((milliseconds % (1000*60*60)) % (1000*60)) / 1000;
	auto milli =  (((milliseconds % (1000*60*60)) % (1000*60)) % 1000);
	ss << hours << "h " << mins << "m " << seconds << "s " << milli << "ms";

	return ss.str();
}

std::map<std::string, std::size_t> readPageCountsFile(std::string path)
{
	std::ifstream istr(path);
	std::map<std::string, std::size_t> rtn;

	while(!istr.eof())
	{
		std::string filename;
		std::size_t count;
		istr >> filename >> count;

		rtn.insert({ filename, count });
	}

	return rtn;
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

	// init xerces
	xercesc::XMLPlatformUtils::Initialize();

	// scan for xml files in the input-folder
	std::vector<fs::path> xmlFileList;
	for(auto dir_it = fs::directory_iterator(inputFolder); dir_it != fs::directory_iterator(); dir_it++)
	{
		if(!fs::is_directory(dir_it->path()))
			xmlFileList.push_back(dir_it->path());
	}

	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Found the following files: " << std::endl;
	for (auto el : xmlFileList) {
		std::cout << el << std::endl;
	}

	// setup and run the handler for running over all entrys in xml file and extracting titles and dates

	auto startTime = std::chrono::steady_clock::now();

	std::vector<std::future<ArticleDatesAndCategoriesHandler>> futures;
	futures.reserve(xmlFileList.size());
	for (auto xmlPath : xmlFileList) 
		futures.emplace_back(std::async(std::launch::async, ParserWrapper(xmlPath,pageCounts)));

	std::map<std::string,Date> articlesWithDates;
	std::vector<std::string> categories;
	std::map<std::string,std::string> redirects;

	for (auto& future : futures)
	{
		auto hand = future.get();
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
	std::ofstream articles_file((outputFolder / "articles_with_dates.txt").string());	
	for(auto article : articlesWithDates)
		articles_file << article.first << "\t" << Date::serialize(article.second) << std::endl;

	std::ofstream categories_file((outputFolder / "categories.txt").string());	
	for(auto cat : categories)
		categories_file << cat << std::endl;

	std::ofstream redirects_file((outputFolder / "redirects.txt").string());	
	for (auto redirect : redirects) 
		redirects_file << redirect.first << "\t" << redirect.second << std::endl;	

	endTime = std::chrono::steady_clock::now();
	diff = std::chrono::duration<double, std::milli>(endTime-startTime).count();
	timings.push_back({ "Writing output files", diff });

	xercesc::XMLPlatformUtils::Terminate();

	endTime = std::chrono::steady_clock::now();
	diff = std::chrono::duration<double, std::milli>(endTime-globalStartTime).count();
	timings.push_back({ "Total", diff });
	
	// short feedback to user
	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Found " << " articles of which " 
		<< articlesWithDates.size() << " had extractable dates. Also found " 
		<< categories.size() << " categories and " << redirects.size() << "." << std::endl;
	
	// output timings
	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Timings: " << std::endl << std::endl;
	for(auto timing : timings)
		std::cout << std::left << std::setw(32) << timing.first + ": " << timingToReadable(timing.second) << std::endl;

	std::cout << "-----------------------------------------------------------------------" << std::endl;



	return 0;
}
