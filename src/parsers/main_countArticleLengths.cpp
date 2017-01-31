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
#include "xercesHandlers/wikiDumpHandler.h"
#include "wikiArticleHandlers/countArticleLengthsHandler.h"
#include "fileNames.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;


int main(int argc, char* argv[])
{
	// setup timings stuff
	auto globalStartTime = std::chrono::steady_clock::now();
	std::vector<std::pair<std::string, double>> timings;

	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "Produce help message.")
		("input-xml-folder", po::value<std::string>(), "The folder that should be scanned for wikidump .xml files.")
		("test-input-file", po::value<std::string>(), "The folder that should be scanned for wikidump .xml files.")
		("page-counts-file", po::value<std::string>(), "The file that contains counts of pages for each .xml file.")
		("output-folder", po::value<std::string>(), "The folder in which the results (articleLengths.txt) should be stored.")
		;
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	// display help if --help was specified
	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return 0;
	}

	if(vm.count("test-input-file"))
	{
		std::ifstream file(vm["test-input-file"].as<std::string>());
		std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

		std::string cleaned = CountArticleLengthHandler::preprocess(content);
		std::cout << cleaned << std::endl;

		std::cout << "--------------------------------------------------------" << std::endl;
		std::cout << "--------------------------------------------------------" << std::endl;
		std::cout << "--------------------------------------------------------" << std::endl;
		std::cout << "--------------------------------------------------------" << std::endl;

		auto prop = CountArticleLengthHandler::compute_properties(cleaned);
		std::cout << "Size: " << std::get<0>(prop) << " Words: " << std::get<1>(prop) << " Sentences: " << std::get<2>(prop) << std::endl;

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

	std::vector<std::future<std::map<std::string, CountArticleLengthHandler::ArticleProperties>>> futures;
	futures.reserve(xmlFileList.size());
	for (auto xmlPath : xmlFileList) 
		futures.emplace_back(std::async(std::launch::async, [&outputFolder, xmlPath, &pageCounts](){ 
			CountArticleLengthHandler artHandler; 

			xercesc::SAX2XMLReader* parser = xercesc::XMLReaderFactory::createXMLReader();
			parser->setFeature(xercesc::XMLUni::fgSAX2CoreValidation, true);
			parser->setFeature(xercesc::XMLUni::fgSAX2CoreNameSpaces, true);   // optional
			parser->setFeature(xercesc::XMLUni::fgXercesSchema , false);   // optional

			// set up call back handlers
			WikiDumpHandler handler(artHandler, true);
			// handler.ProgressReportInterval = 100;
			handler.TitleFilter = [](const std::string& title) {
			return !(
				title.substr(0,5) == "User:" 
				|| title.substr(0,10) == "Wikipedia:" 
				|| title.substr(0,5) == "Talk:" 
				|| title.substr(0,5) == "File:" 
				|| title.substr(0,14) == "Category talk:" 
				|| title.substr(0,14) == "Template talk:"
				|| title.substr(0,9) == "Template:"
				|| title.substr(0,10) == "User talk:"
				|| title.substr(0,10) == "File talk:"
				|| title.substr(0,15) == "Wikipedia talk:"
				);
			};
			handler.ProgressCallback = std::bind(printProgress, pageCounts, xmlPath, std::placeholders::_1);

			parser->setContentHandler(&handler);
			parser->setErrorHandler(&handler);

			// run parser
			parser->parse(xmlPath.c_str());
			delete parser;
	
			auto xmlFileName = xmlPath.stem();
			auto path = outputFolder / xmlFileName;
			path.replace_extension(".txt");
			std::ofstream lengths_file(path.string());	
			std::cout << "Writing to " << path << std::endl;
			for(auto length : artHandler.article_counts)
			{
				lengths_file << "\"" << length.first << "\",";
				lengths_file << std::get<0>(length.second) << ",";
				lengths_file << std::get<1>(length.second) << ",";
				lengths_file << std::get<2>(length.second);
				lengths_file << std::endl;
			}

			return artHandler.article_counts;
		}));

	// std::map<std::string, CountArticleLengthHandler::ArticleProperties> extracted_lengths;
	for (auto& future : futures)
	{
		auto lengths = future.get();

	}

	auto endTime = std::chrono::steady_clock::now();
	auto diff = std::chrono::duration<double, std::milli>(endTime-startTime).count();
	timings.push_back({ "Parsing .xml files and merging data structures.", diff });

	startTime = std::chrono::steady_clock::now();
	
	// std::ofstream lengths_file((outputFolder / "article_lengths.txt").string());	
	// for(auto length : extracted_lengths)
	// {
		// lengths_file << "\"" << length.first << "\",";
		// lengths_file << std::get<0>(length.second) << ",";
		// lengths_file << std::get<1>(length.second) << ",";
		// lengths_file << std::get<2>(length.second);
		// lengths_file << std::endl;
	// }
	timings.push_back({ "Writing output files", diff });

	xercesc::XMLPlatformUtils::Terminate();

	endTime = std::chrono::steady_clock::now();
	diff = std::chrono::duration<double, std::milli>(endTime-globalStartTime).count();
	timings.push_back({ "Total", diff });
	
	// short feedback to user
	std::size_t totalArticleNumber = 0;
	for (auto path : xmlFileList) {
		auto it = pageCounts.find(path.filename().c_str());
		if(it != pageCounts.end())
			totalArticleNumber += it->second;
	}
	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Status: " << std::endl;
	std::cout << std::left << std::setw(40) << "Total number of articles: " << totalArticleNumber << std::endl;
	
	// output timings
	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Timings: " << std::endl << std::endl;
	for(auto timing : timings)
		std::cout << std::left << std::setw(50) << timing.first + ": " << timingToReadable(timing.second) << std::endl;

	std::cout << "-----------------------------------------------------------------------" << std::endl;

	return 0;
}
