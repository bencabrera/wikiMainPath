// stdlib
#include <iostream>
#include <iomanip>
#include <map>
#include <future>
#include <thread>
#include <fstream>
#include <functional>
#include <mutex>

// xerces
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>

// boost
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

// local files
#include "helpers/shared.h"
#include "helpers/printProgress.h"
#include "../core/wikiDataCache.h"

// wiki xml dump lib
#include "wikiArticleHandlers/articleDatesAndCategoriesHandler.h"
#include "../../libs/wiki_xml_dump_xerces/src/parsers/parallelParser.hpp"
#include "../../libs/wiki_xml_dump_xerces/src/parsers/singleCoreParser.hpp"
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
		("input-paths-file", po::value<std::string>(), "File of which each line is absolute path to an xml file that is part of a Wikipedia dump.")
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

	if(!vm.count("input-paths-file") || !vm.count("output-folder"))
	{
		std::cerr << "Please specify the parameters --input-paths-file and --output-folder." << std::endl;
		return 1;
	}

	const std::string paths_file_path = vm["input-paths-file"].as<std::string>();
	const fs::path outputFolder(vm["output-folder"].as<std::string>());

	auto pageCounts = (vm.count("page-counts-file") ? readPageCountsFile(vm["page-counts-file"].as<std::string>()) : std::map<std::string, std::size_t>());

	if(!fs::is_directory(outputFolder))
	{
		std::cerr << "Parameter --ouput-folder is no folder, creating it..." << std::endl;
		fs::create_directory(outputFolder);
	}

	// scan for xml files in the input-folder
	std::ifstream paths_file(paths_file_path);
	auto paths = read_lines_from_file(paths_file);

	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Found the following files: " << std::endl;
	for (auto f : paths) 
		std::cout << f << std::endl;

	// setup and run the handler for running over all entrys in xml file and extracting titles and dates

	timer.start_timing_step("parsing", "Parsing .xml files and merging data structures.", &std::cout);

	// init xerces
	xercesc::XMLPlatformUtils::Initialize();

	// setup files and mutices
	std::ofstream articles_file((outputFolder / WikiDataCache::ARTICLES_TITLES_FILE).string());	
	std::ofstream categories_file((outputFolder / WikiDataCache::CATEGORIES_TITLES_FILE).string());	
	std::ofstream redirects_file((outputFolder / WikiDataCache::REDIRECTS_FILE).string());	
	std::ofstream article_dates_file((outputFolder / WikiDataCache::ARTICLE_DATES_FILE).string());	

	std::mutex article_titles_mutex, category_titles_mutex, redirect_mutex;


	WikiXmlDumpXerces::WikiDumpHandlerProperties parser_properties;
	parser_properties.TitleFilter = WikiXmlDumpXerces::only_articles_and_categories();


	auto start_time = std::chrono::steady_clock::now();
	std::size_t total_count = 0;
	std::map<std::string, std::size_t> current_counts;
	std::mutex mutex;

	if(!pageCounts.empty())
	{
		for(const auto& p : pageCounts)
		{
			total_count += p.second;
			current_counts[p.first] = 0;
		}

		parser_properties.ProgressCallback = std::bind(print_progress, std::ref(mutex), std::ref(current_counts), std::cref(total_count), std::cref(start_time), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		parser_properties.ProgressReportInterval = 5000;
	}

	// WikiXmlDumpXerces::ParallelParser<ArticleDatesAndCategoriesHandler> parser([](){ return ArticleDatesAndCategoriesHandler(&std::cout); }, parser_properties);
	// WikiXmlDumpXerces::ParallelParser<ArticleDatesAndCategoriesHandler> parser([&articles_file, &categories_file, &redirects_file, &article_dates_file, &article_titles_mutex, &category_titles_mutex, &redirect_mutex](){ return ArticleDatesAndCategoriesHandler(articles_file,article_dates_file,categories_file,redirects_file,article_titles_mutex,category_titles_mutex,redirect_mutex); }, parser_properties);
	ArticleDatesAndCategoriesHandler handler(articles_file,article_dates_file,categories_file,redirects_file,article_titles_mutex,category_titles_mutex,redirect_mutex);
	WikiXmlDumpXerces::SingleCoreParser parser(handler, parser_properties);
	parser.Run(paths.begin(), paths.end());

	// terminate xerces
	xercesc::XMLPlatformUtils::Terminate();

	// std::map<std::string,std::vector<Date>> articlesWithDates;
	// std::vector<std::string> category_titles;
	// std::map<std::string,std::string> redirects;
	// std::size_t n_errors = 0;

	// for (auto& hand : parser.PageHandlers())
	// {
	// articlesWithDates.insert(hand.articles.begin(), hand.articles.end());
	// category_titles.insert(category_titles.end(), hand.categories.begin(), hand.categories.end());
	// redirects.insert(hand.redirects.begin(), hand.redirects.end());
	// n_errors += hand.n_errors;
	// }


	timer.stop_timing_step("parsing");


	// timer.start_timing_step("sorting", "Sorting categories vector", &std::cout);
	// std::sort(category_titles.begin(), category_titles.end());
	// timer.stop_timing_step("sorting");

	// timer.start_timing_step("output", "Writing output files", &std::cout);

	// WikiDataCache wiki_data_cache(outputFolder.string());
	// wiki_data_cache.write_article_titles(articlesWithDates);
	// wiki_data_cache.write_category_titles(category_titles);
	// wiki_data_cache.write_article_dates(articlesWithDates);
	// wiki_data_cache.write_redirects(redirects);

	// timer.stop_timing_step("output");

	timer.stop_timing_step("global");

	// short feedback to user
	std::size_t totalArticleNumber = 0;
	for (auto path : paths) {
		auto it = pageCounts.find(path);
		if(it != pageCounts.end())
			totalArticleNumber += it->second;
	}
	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Status: " << std::endl;
	std::cout << std::left << std::setw(40) << "Total number of pages: " << totalArticleNumber << std::endl;
	// std::cout << std::left << std::setw(40) << "Articles with extractable dates: " << articlesWithDates.size() << std::endl;
	// std::cout << std::left << std::setw(40) << "Number of date extraction errors: " << n_errors << std::endl;
	// std::cout << std::left << std::setw(40) << "Categories extracted: " << category_titles.size() << std::endl;
	// std::cout << std::left << std::setw(40) << "Redirects extracted: " << redirects.size() << std::endl;

	// output timings
	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Timings: " << std::endl << std::endl;
	timer.print_timings(std::cout);

	std::cout << "-----------------------------------------------------------------------" << std::endl;



	return 0;
}
