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
#include "parserWrappers/s3_wrapper.h"
#include "parserWrappers/s3_recursiveFillCategories.h"
#include "fileNames.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace WikiMainPath;

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, boost::no_property, boost::no_property, boost::vecS> Graph;

std::vector<boost::container::flat_set<std::size_t>> readDataFromFile(const fs::path& inputFolder, std::vector<std::string>& categories)
{

	std::ifstream categories_file((inputFolder / CATEGORIES_FILE).string());	
	std::ifstream categories_has_article_file((inputFolder / CAT_HAS_ARTICLE_FILE).string());	
		
	std::string line;
	while(std::getline(categories_file, line))
	{
		boost::trim(line);
		categories.push_back(line);
	}

	std::vector<boost::container::flat_set<std::size_t>> rtn(categories.size());
	std::size_t iCategory = 0;
	while(std::getline(categories_has_article_file, line))
	{
		std::stringstream ss(line);
		std::string tmp;
		while(!ss.eof())
		{
			ss >> tmp;
			boost::trim(tmp);

			if(tmp != "")
				rtn[iCategory].insert(std::stoi(tmp));
		}

		iCategory++;
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
	
	std::vector<std::string> categories;
	auto category_has_article = readDataFromFile(inputArticleFolder, categories);	

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

	VectorMutex<1000> vecMutex;
	std::vector<std::future<void>> futures;
	Graph graph(categories.size());
	futures.reserve(xmlFileList.size());
	for (auto xmlPath : xmlFileList) 
		futures.emplace_back(std::async(std::launch::async, S3ParserWrapper(xmlPath, pageCounts, categories, graph, vecMutex)));

	for (auto& future : futures)
		future.get();

	endTime = std::chrono::steady_clock::now();
	diff = std::chrono::duration<double, std::milli>(endTime-startTime).count();
	timings.push_back({ "Parsing .xml files.", diff });

	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Running over graph and adding categories recursively" << std::endl;
	startTime = std::chrono::steady_clock::now();

	recursiveFillCategories(graph, category_has_article);	

	endTime = std::chrono::steady_clock::now();
	diff = std::chrono::duration<double, std::milli>(endTime-startTime).count();
	timings.push_back({ "Running over graph and adding categories recursively", diff });

	startTime = std::chrono::steady_clock::now();
	std::ofstream catArtFile((outputFolder / CAT_HAS_ARTICLE_FILE).string());	
	for(std::size_t i = 0; i < category_has_article.size(); i++)
	{
		for (auto art : category_has_article[i]) 
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
