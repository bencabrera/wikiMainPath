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
#include "../core/wikiDataCache.h"
#include "helpers/s3_recursiveFillCategories.h"

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
		("input-xml-folder", po::value<std::string>(), "The folder that should be scanned for wikidump .xml files.")
		("page-counts-file", po::value<std::string>(), "The file that contains counts of pages for each .xml file.")
		("selection-file", po::value<std::string>(), "The file that contains a list of all .xml files which should be considered.")
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

	const fs::path inputXmlFolder(vm["input-xml-folder"].as<std::string>());
	const fs::path outputFolder(vm["output-folder"].as<std::string>());

	auto pageCounts = (vm.count("page-counts-file") ? readPageCountsFile(vm["page-counts-file"].as<std::string>()) : std::map<std::string, std::size_t>());

	if(!fs::is_directory(inputXmlFolder))
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
	std::vector<std::string> paths;
	if(vm.count("selection-file"))
		paths = selected_filename_in_folder(inputXmlFolder, fs::path(vm["selection-file"].as<std::string>()));
	else
		paths = selected_filename_in_folder(inputXmlFolder);
		

	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Found the following files: " << std::endl;
	for (auto f : paths) 
		std::cout << f << std::endl;

	timer.start_timing_step("reading", "Reading in already parsed files... ", &std::cout);

	// setup existing data and containers for upcoming
	WikiDataCache wiki_data_cache(outputFolder.string());
	const auto& article_titles = wiki_data_cache.article_titles();
	const auto& category_titles = wiki_data_cache.category_titles();
	const auto& redirects = wiki_data_cache.redirects();

	std::vector<boost::container::flat_set<std::size_t>> category_has_article(category_titles.size());
	AllLinksArticleHander::CategoryHirachyGraph category_hirachy_graph(category_titles.size());
	std::vector<boost::container::flat_set<std::size_t>> article_adjacency_list(article_titles.size());
	VectorMutex<1000> vecMutex;

	timer.stop_timing_step("reading");

	// setup and run the handler for running over all entrys in xml file and extracting titles and dates
	timer.start_timing_step("parsing", "Parsing .xml files", &std::cout);

	// init xerces
	xercesc::XMLPlatformUtils::Initialize();

	WikiXmlDumpXerces::WikiDumpHandlerProperties parser_properties;
	parser_properties.TitleFilter = WikiXmlDumpXerces::only_articles_and_categories(); 		
	parser_properties.ProgressCallback = std::bind(printProgress, pageCounts, std::placeholders::_2, std::placeholders::_1, std::placeholders::_3);

	WikiXmlDumpXerces::ParallelParser<AllLinksArticleHander> parser([&article_titles, &category_titles, &redirects, &category_has_article, &category_hirachy_graph, &article_adjacency_list, &vecMutex](){ 
		return AllLinksArticleHander(article_titles, category_titles, redirects, category_has_article, category_hirachy_graph, article_adjacency_list, vecMutex); 
	}, parser_properties);
	// CategoryHasArticleHandler handler(articles, categories, categoryHasArticle, vecMutex);
	// WikiXmlDumpXerces::SingleCoreParser parser(handler, parser_properties);

	parser.Run(paths.begin(), paths.end());

	// terminate xerces
	xercesc::XMLPlatformUtils::Terminate();

	timer.stop_timing_step("parsing");

	// computing recursive category_has_article
	timer.start_timing_step("recursive", "Computing recursive category_has_article", &std::cout);

	std::ofstream graphFile((outputFolder / "category_hirachy_graph.txt").string());	
	for (auto v : boost::make_iterator_range(boost::vertices(category_hirachy_graph)))
	{
		for (auto e : boost::make_iterator_range(boost::out_edges(v,category_hirachy_graph))) 
			graphFile << boost::target(e,category_hirachy_graph) << " ";

		graphFile << std::endl;
	}

	recursiveFillCategories(category_hirachy_graph, category_has_article);	
	timer.stop_timing_step("recursive");

	timer.start_timing_step("output", "Writing output files", &std::cout);
	wiki_data_cache.write_category_has_article(category_has_article);
	wiki_data_cache.write_article_network(article_adjacency_list);


	timer.stop_timing_step("output");

	timer.stop_timing_step("global");

	// output timings
	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Timings: " << std::endl << std::endl;
	timer.print_timings(std::cout);

	std::cout << "-----------------------------------------------------------------------" << std::endl;



	return 0;
}
