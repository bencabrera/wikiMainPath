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
#include "../core/wikiDataCache.h"

// wiki xml dump lib
#include "wikiArticleHandlers/articleDatesAndCategoriesHandler.h"
#include "../../libs/wiki_xml_dump_xerces/src/parsers/parallelParser.hpp"
#include "../../libs/wiki_xml_dump_xerces/src/handlers/wikiDumpHandlerProperties.hpp"
#include "../../libs/wiki_xml_dump_xerces/src/handlers/basicTitleFilters.hpp"

// shared library
#include "../../libs/shared/cpp/stepTimer.hpp"

namespace po = boost::program_options;
namespace fs = boost::filesystem;


int main(int argc, char* argv[])
{
	using namespace WikiMainPath;

	// setup timings stuff
	Shared::StepTimer timer;
	timer.start_timing_step("global","Total");

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

	timer.start_timing_step("parsing", "Parsing .xml files and merging data structures.", &std::cout);

	// init xerces
	xercesc::XMLPlatformUtils::Initialize();

	WikiXmlDumpXerces::WikiDumpHandlerProperties parser_properties;
	parser_properties.TitleFilter = WikiXmlDumpXerces::only_articles_and_categories();
	parser_properties.ProgressCallback = std::bind(printProgress, pageCounts, std::placeholders::_2, std::placeholders::_1, std::placeholders::_3);
	parser_properties.ProgressReportInterval = 5000;

	// WikiXmlDumpXerces::ParallelParser<ArticleDatesAndCategoriesHandler> parser([](){ return ArticleDatesAndCategoriesHandler(&std::cout); }, parser_properties);
	WikiXmlDumpXerces::ParallelParser<ArticleDatesAndCategoriesHandler> parser([](){ return ArticleDatesAndCategoriesHandler(); }, parser_properties);
	parser.Run(paths.begin(), paths.end());

	// terminate xerces
	xercesc::XMLPlatformUtils::Terminate();

	std::map<std::string,std::vector<Date>> articlesWithDates;
	std::vector<std::string> category_titles;
	std::map<std::string,std::string> redirects;
	std::size_t n_errors = 0;

	for (auto& hand : parser.PageHandlers())
	{
		articlesWithDates.insert(hand.articles.begin(), hand.articles.end());
		category_titles.insert(category_titles.end(), hand.categories.begin(), hand.categories.end());
		redirects.insert(hand.redirects.begin(), hand.redirects.end());
		n_errors += hand.n_errors;
	}


	timer.stop_timing_step("parsing");


	timer.start_timing_step("sorting", "Sorting categories vector", &std::cout);
	std::sort(category_titles.begin(), category_titles.end());
	timer.stop_timing_step("sorting");

	timer.start_timing_step("output", "Writing output files", &std::cout);

	WikiDataCache wiki_data_cache(outputFolder.string());
	wiki_data_cache.write_article_titles(articlesWithDates);
	wiki_data_cache.write_category_titles(category_titles);
	wiki_data_cache.write_article_dates(articlesWithDates);
	wiki_data_cache.write_redirects(redirects);

	timer.stop_timing_step("output");

	timer.stop_timing_step("global");
	
	// short feedback to user
	std::size_t totalArticleNumber = 0;
	for (auto path : xmlFileList) {
		auto it = pageCounts.find(path.filename().c_str());
		if(it != pageCounts.end())
			totalArticleNumber += it->second;
	}
	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Status: " << std::endl;
	std::cout << std::left << std::setw(40) << "Total number of pages: " << totalArticleNumber << std::endl;
	std::cout << std::left << std::setw(40) << "Articles with extractable dates: " << articlesWithDates.size() << std::endl;
	std::cout << std::left << std::setw(40) << "Number of date extraction errors: " << n_errors << std::endl;
	std::cout << std::left << std::setw(40) << "Categories extracted: " << category_titles.size() << std::endl;
	std::cout << std::left << std::setw(40) << "Redirects extracted: " << redirects.size() << std::endl;
	
	// output timings
	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Timings: " << std::endl << std::endl;
	timer.print_timings(std::cout);

	std::cout << "-----------------------------------------------------------------------" << std::endl;



	return 0;
}
