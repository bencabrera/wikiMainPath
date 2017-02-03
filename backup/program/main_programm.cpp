// stdlib
#include <iostream>
#include <iomanip>
#include <map>
#include <future>
#include <thread>
#include <fstream>

// boost
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>

// local files
#include "fullTextSearch.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

void readDataFromFile (const fs::path& inputFolder, std::vector<std::string>& articles, std::vector<std::tm>& dates, std::map<std::string, std::vector<std::string>>& categoriesToArticles, std::map<std::string, std::string>& redirects)
{
	std::ifstream articles_file((inputFolder / "articles_with_dates.txt").string());	
	std::ifstream categories_file((inputFolder / "categories.txt").string());	
	std::ifstream redirects_file((inputFolder / "redirects.txt").string());	
		
	std::string line;
	while(std::getline(articles_file, line))
	{
		std::istringstream ss(line);
		std::string title, dateStr;
		std::getline(ss, title, '\t');
		std::getline(ss, dateStr, '\t');
		articles.push_back(title);
	}			
}



int main(int argc, char* argv[])
{
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "Produce help message.")
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

	if(!vm.count("input-article-folder"))
	{
		std::cerr << "Please specify the parameters --input-article-folder." << std::endl;
		return 1;
	}

	const fs::path inputArticleFolder(vm["input-article-folder"].as<std::string>());

	if(!fs::is_directory(inputArticleFolder))
	{
		std::cerr << "Parameter --input-xml-folder is no folder." << std::endl;
		return 1;
	}

	if(!fs::exists(inputArticleFolder / "articles_with_dates.txt") || !fs::exists(inputArticleFolder / "categories.txt") || !fs::exists(inputArticleFolder / "redirects.txt"))
	{
		std::cerr << "The input article folder does not contain all necessary files." << std::endl;
		return 1;
	}

	std::vector<std::string> articles;
	std::vector<std::tm> dates;
	std::map<std::string, std::vector<std::string>> categoriesToArticles;
	std::map<std::string, std::string> redirects;

	readDataFromFile(inputArticleFolder, articles, dates, categoriesToArticles, redirects);

	auto invertedIndex = buildInvertedIndex(articles);

	while(true)
	{
		std::string queryStr;
		std::cout << std::endl << "Please enter your search query: ";
		std::getline(std::cin, queryStr);

		auto docs = query(invertedIndex, queryStr);

		std::cout << "---------------------------------------------------" << std::endl;
		std::cout << "Query: " << queryStr << std::endl << std::endl;
		std::cout << "Results: " << std::endl;
		std::cout << stem(queryStr) << std::endl;
		//for (auto docIdx : docs) {
		//	std::cout << articles[docIdx] << std::endl;
		//}
	}

	return 0;
}
