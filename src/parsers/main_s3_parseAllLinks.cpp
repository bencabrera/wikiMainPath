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
#include "helpers/shared.h"
#include "helpers/printProgress.h"
#include "../core/wikiDataCache.h"
#include "helpers/removeCyclesInCategoryHirachy.h"

// wiki xml dump lib
#include "wikiArticleHandlers/allLinksArticleHandler.h"
#include "../../libs/wiki_xml_dump_xerces/src/parsers/parallelParser.hpp"
#include "../../libs/wiki_xml_dump_xerces/src/handlers/wikiDumpHandlerProperties.hpp"
#include "../../libs/wiki_xml_dump_xerces/src/handlers/basicTitleFilters.hpp"
#include "../../libs/wiki_xml_dump_xerces/src/parsers/singleCoreParser.hpp"

// shared library
#include "../../libs/shared/cpp/stepTimer.hpp"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace WikiMainPath;

int main(int argc, char* argv[])
{
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

	// init xerces
	xercesc::XMLPlatformUtils::Initialize();

	// scan for xml files in the input-folder
	std::ifstream paths_file(paths_file_path);
	auto paths = read_lines_from_file(paths_file);

	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Found the following files: " << std::endl;
	for (auto f : paths) 
		std::cout << f << std::endl;

	// setup existing data and containers for upcoming
	timer.start_timing_step("reading", "Reading in already parsed files... ", &std::cout);
	WikiDataCache wiki_data_cache(outputFolder.string());
	const auto& article_titles = wiki_data_cache.article_titles();
	const auto& category_titles = wiki_data_cache.category_titles();
	const auto& redirects = wiki_data_cache.redirects();

	std::vector<boost::container::flat_set<std::size_t>> category_has_article(category_titles.size());
	CategoryHirachyGraph category_hirachy_graph(category_titles.size());
	std::vector<boost::container::flat_set<std::size_t>> article_adjacency_list(article_titles.size());
	VectorMutex<1000> vecMutex;
	timer.stop_timing_step("reading", &std::cout);

	
	// setup and run the handler for running over all entrys in xml file and extracting titles and dates
	timer.start_timing_step("parsing", "Parsing .xml files", &std::cout);

	// init xerces
	xercesc::XMLPlatformUtils::Initialize();

	WikiXmlDumpXerces::WikiDumpHandlerProperties parser_properties;
	parser_properties.TitleFilter = WikiXmlDumpXerces::only_articles_and_categories(); 		


	auto start_time = std::chrono::steady_clock::now();
	std::size_t total_count = 0;
	std::map<std::string, std::size_t> current_counts;
	std::mutex mutex;

	if(!pageCounts.empty())
	{
		std::set<std::string> used_paths(paths.begin(), paths.end());
		for(const auto& p : pageCounts)
		{
			if(used_paths.count(p.first) > 0)
			{
				total_count += p.second;
				current_counts[p.first] = 0;
			}
		}

		parser_properties.ProgressCallback = std::bind(print_progress, std::ref(mutex), std::ref(current_counts), std::cref(total_count), std::cref(start_time), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		parser_properties.ProgressReportInterval = 5000;
	}

	WikiXmlDumpXerces::ParallelParser<AllLinksArticleHander> parser([&article_titles, &category_titles, &redirects, &category_has_article, &category_hirachy_graph, &article_adjacency_list, &vecMutex](){ 
		return AllLinksArticleHander(article_titles, category_titles, redirects, category_has_article, category_hirachy_graph, article_adjacency_list, vecMutex); 
	}, parser_properties);
	// CategoryHasArticleHandler handler(articles, categories, categoryHasArticle, vecMutex);
	// WikiXmlDumpXerces::SingleCoreParser parser(handler, parser_properties);

	parser.Run(paths.begin(), paths.end());

	// terminate xerces
	xercesc::XMLPlatformUtils::Terminate();

	timer.stop_timing_step("parsing", &std::cout);

	// write article network
	timer.start_timing_step("write_article_network", "Writing article network", &std::cout);
	wiki_data_cache.write_article_network(article_adjacency_list);
	timer.stop_timing_step("write_article_network", &std::cout);

	// write category has article in the non-recursive version
	timer.start_timing_step("write_category_has_article_nonrecursive", "Writing non-recursive category_has_article", &std::cout);
	wiki_data_cache.write_category_has_article(category_has_article);
	timer.stop_timing_step("write_category_has_article_nonrecursive", &std::cout);

	// write category_hirachy_graph
	timer.start_timing_step("write_hirachy_graph", "Writing category_hirachy_graph", &std::cout);
	wiki_data_cache.write_category_hirachy_graph(category_hirachy_graph);
	timer.stop_timing_step("write_hirachy_graph", &std::cout);

	// computing recursive category_has_article
	timer.start_timing_step("remove_cycles", "Removing cycles from category_hirachy_graph", &std::cout);
	remove_cycles_in_category_hirachy(category_hirachy_graph);	
	timer.stop_timing_step("remove_cycles", &std::cout);

	// write category_hirachy_graph
	timer.start_timing_step("write_hirachy_graph2", "Writing category_hirachy_graph without cycles", &std::cout);
	wiki_data_cache.write_category_hirachy_graph(category_hirachy_graph);
	timer.stop_timing_step("write_hirachy_graph2", &std::cout);

	// // computing recursive category_has_article
	// timer.start_timing_step("compute_recursive_cha", "Computing recursive category_has_article", &std::cout);
	// recursiveFillCategories(category_hirachy_graph, category_has_article);	
	// timer.stop_timing_step("compute_recursive_cha", &std::cout);

	// // write final category_has_article file
	// timer.start_timing_step("final_cha", "Writing final category_has_article", &std::cout);
	// wiki_data_cache.write_category_has_article(category_has_article);
	// timer.stop_timing_step("final_cha", &std::cout);


	timer.stop_timing_step("global");

	// output timings
	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Timings: " << std::endl << std::endl;
	timer.print_timings(std::cout);

	std::cout << "-----------------------------------------------------------------------" << std::endl;



	return 0;
}
