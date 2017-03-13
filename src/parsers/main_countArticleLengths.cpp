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

// boost.spirit stuff
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_repeat.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/std_pair.hpp>

// local files
#include "shared.h"
#include "wikiArticleHandlers/countArticleLengthsHandler.h"

#include "../../libs/wiki_xml_dump_xerces/src/parsers/parallelParser.hpp"
#include "../../libs/wiki_xml_dump_xerces/src/handlers/wikiDumpHandlerProperties.hpp"


namespace po = boost::program_options;
namespace fs = boost::filesystem;

std::vector<std::pair<std::string, std::vector<std::string>>> read_existing_results_file(const fs::path& file_path)
{
	using namespace boost::spirit::qi;
	using namespace boost::phoenix;

	std::ifstream file(file_path.string());

	std::stringstream buffer;
	buffer << file.rdbuf();

	boost::spirit::qi::rule<std::string::const_iterator, std::vector<std::pair<std::string, std::vector<std::string>>>()> lines;
	boost::spirit::qi::rule<std::string::const_iterator, std::pair<std::string, std::vector<std::string>>()> line;
	boost::spirit::qi::rule<std::string::const_iterator, std::vector<std::string>()> params;
	boost::spirit::qi::rule<std::string::const_iterator, std::string()> param;

	lines = line >> line [push_back(_val, boost::spirit::_1)] % eol;
	line = *(char_ - ';') [at_c<0>(_val) += boost::spirit::_1] >> lit(';') >> params [at_c<1>(_val) = boost::spirit::_1];
	params = param [push_back(_val, boost::spirit::_1)] % lit(';');
	param = *(char_ - ';' - eol) [_val += boost::spirit::_1];

	std::string str = buffer.str();	
	auto it = str.cbegin();

	// BOOST_SPIRIT_DEBUG_NODE(line);
	// BOOST_SPIRIT_DEBUG_NODE(params);
	// BOOST_SPIRIT_DEBUG_NODE(param);

	std::vector<std::pair<std::string, std::vector<std::string>>> rtn;
	parse(it, str.cend(), lines, rtn);

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
		("test-input-file", po::value<std::string>(), "The folder that should be scanned for wikidump .xml files.")
		("page-counts-file", po::value<std::string>(), "The file that contains counts of pages for each .xml file.")
		("output-folder", po::value<std::string>(), "The folder in which the results (articleLengths.txt) should be stored.")
		("existing-results-folder", po::value<std::string>(), "The folder in which the results (articleLengths.txt) should be stored.")
		;
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	// display help if --help was specified
	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return 0;
	}

	if(!vm.count("input-xml-folder") || !vm.count("output-folder") || !vm.count("existing-results-folder"))
	{
		std::cerr << "Please specify the parameters --input-xml-folder and --output-folder." << std::endl;
		return 1;
	}

	const fs::path inputFolder(vm["input-xml-folder"].as<std::string>());
	const fs::path outputFolder(vm["output-folder"].as<std::string>());
	const fs::path existing_path(vm["existing-results-folder"].as<std::string>());

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



	std::vector<fs::path> existing_results_file_list;
	for(auto dir_it = fs::directory_iterator(existing_path); dir_it != fs::directory_iterator(); dir_it++)
	{
		if(!fs::is_directory(dir_it->path()))
			existing_results_file_list.push_back(dir_it->path());
	}


	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Found the following existing result files: " << std::endl;
	std::vector<std::map<std::string, std::vector<std::string>>> existing_results;
	for (auto el : existing_results_file_list) {
		std::cout << el << std::endl;

		auto tmp = read_existing_results_file(el);
		existing_results.emplace_back(tmp.begin(), tmp.end());
	}
	std::size_t oldColCount = (existing_results[0].begin()->second).size();

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

	auto startTime = std::chrono::steady_clock::now();


	// init xerces
	xercesc::XMLPlatformUtils::Initialize();

	WikiXmlDumpXerces::WikiDumpHandlerProperties parser_properties;
	parser_properties.TitleFilter = [](const std::string& title) {
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
		
	parser_properties.ProgressCallback = std::bind(printProgress, pageCounts, "bla", std::placeholders::_1);

	WikiXmlDumpXerces::ParallelParser<CountArticleLengthHandler> parser([&existing_results](){ 
		return CountArticleLengthHandler(existing_results); 
	}, parser_properties);
	parser.Run(paths.begin(), paths.end());

	// terminate xerces
	xercesc::XMLPlatformUtils::Terminate();


	auto endTime = std::chrono::steady_clock::now();
	auto diff = std::chrono::duration<double, std::milli>(endTime-startTime).count();
	timings.push_back({ "Parsing .xml files and merging data structures.", diff });

	startTime = std::chrono::steady_clock::now();

	for(std::size_t i = 0; i < existing_results_file_list.size(); i++)
	{
		auto path = outputFolder / existing_results_file_list[i].filename();	
		std::ofstream out_file(path.string());

		out_file << "Article name; Number of users with edits; Number of users with major edits; Number of users on talk page; Number of users with major edits or talk page activity; Similarity; Clustering Coefficient; Gini Coefficient (all edits); Gini coefficient (major edits);Article age (in months);Length(Bytes);Length(Words);Length(Sentences);" << std::endl;
		for (auto line : existing_results[i]) {
			if(line.second.size() == oldColCount)
			{
				line.second.push_back("");
				line.second.push_back("");
				line.second.push_back("");
			}
			out_file << line.first;
			for (auto col : line.second) {
				out_file << ";" << col;	
			}
			out_file << std::endl;
		}
	}

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
