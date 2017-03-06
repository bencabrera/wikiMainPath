// stdlib
#include <iostream>
#include <iomanip>
#include <map>
#include <future>
#include <thread>
#include <fstream>
#include <functional>
#include <utility>

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
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>

// local files
#include "shared.h"
#include "s3_recursiveFillCategories.h"
#include "../core/wikiDataCache.h"

#include "wikiArticleHandlers/categoryRecursiveHandler.h"
#include "../../libs/wiki_xml_dump_xerces/src/parsers/parallelParser.hpp"
#include "../../libs/wiki_xml_dump_xerces/src/handlers/wikiDumpHandlerProperties.hpp"
#include "../../libs/wiki_xml_dump_xerces/src/handlers/basicTitleFilters.hpp"

// shared library
#include "../../libs/shared/cpp/stepTimer.hpp"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace WikiMainPath;

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, boost::no_property, boost::no_property, boost::vecS> Graph;

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
		std::cerr << "Please specify the parameters --input-xml-folder and --input-article-folder and --output-folder." << std::endl;
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
	std::vector<fs::path> xmlFileList;
	for(auto dir_it = fs::directory_iterator(inputXmlFolder); dir_it != fs::directory_iterator(); dir_it++)
	{
		if(!fs::is_directory(dir_it->path()))
			xmlFileList.push_back(dir_it->path());
	}

	timer.start_timing_step("reading", "Reading in already parsed files... ", &std::cout);
	WikiDataCache wiki_data_cache(outputFolder.string());
	const auto& categories = wiki_data_cache.category_titles();
	auto category_has_article = wiki_data_cache.category_has_article_set(); // make copy of it
	timer.stop_timing_step("reading");

	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Found the following .xml files: " << std::endl;

	std::vector<std::string> paths;
	for (auto el : xmlFileList) {
		std::cout << el << std::endl;
		paths.push_back(el.string());
	}

	// setup and run the handler for running over all entrys in xml file and extracting titles and dates
	timer.start_timing_step("parsing", "Parsing .xml files", &std::cout);

	VectorMutex<1000> vecMutex;
	Graph graph(categories.size());
	
	// init xerces
	xercesc::XMLPlatformUtils::Initialize();

	WikiXmlDumpXerces::WikiDumpHandlerProperties parser_properties;
	parser_properties.TitleFilter = WikiXmlDumpXerces::only_categories();		
	parser_properties.ProgressCallback = std::bind(printProgress, pageCounts, std::placeholders::_2, std::placeholders::_1, std::placeholders::_3);

	WikiXmlDumpXerces::ParallelParser<CategoryRecursiveHandler> parser([&categories, &graph, &vecMutex](){ 
		return CategoryRecursiveHandler(categories, graph, vecMutex); 
	}, parser_properties);
	parser.Run(paths.begin(), paths.end());

	// terminate xerces
	xercesc::XMLPlatformUtils::Terminate();

	timer.stop_timing_step("parsing");

	timer.start_timing_step("recursive", "Running over graph and adding categories recursively", &std::cout);
	recursiveFillCategories(graph, category_has_article);	
	timer.stop_timing_step("recursive");

	timer.start_timing_step("output", "Writing output files", &std::cout);
	wiki_data_cache.write_category_has_article(category_has_article);
	timer.stop_timing_step("output");

	timer.stop_timing_step("global");

	// output timings
	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Timings: " << std::endl << std::endl;
	timer.print_timings(std::cout);

	std::cout << "-----------------------------------------------------------------------" << std::endl;



	return 0;
}
