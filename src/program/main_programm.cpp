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

typedef std::map<std::string, std::set<std::size_t>> InvertedIndex;
typedef std::set<std::size_t> DocumentSet;

std::string stem(std::string str)
{
	return boost::to_lower_copy(str);
}

InvertedIndex buildInvertedIndex(const std::vector<std::string>& documents)
{
	InvertedIndex rtn;

	std::size_t iDoc = 0;
	for (auto doc : documents) {
		std::vector<std::string> wordVec;
		boost::split(wordVec, doc, boost::is_any_of(" "));
		for (auto wordStr : wordVec) {
			// stop words and stemming
			std::string word = stem(wordStr);
			
			
			// add to inverted index
			auto it = rtn.find(word);
			if(it != rtn.end())
				it->second.insert(iDoc);
			else
				rtn.insert({ word, { iDoc } });
		}

		iDoc++;
	}

	return rtn;
}

DocumentSet query(const InvertedIndex& invertedIndex, std::string queryStr)
{
	std::vector<std::string> queryWordVec;
	boost::split(queryWordVec, queryStr, boost::is_any_of(" "));

	for (auto& i : queryWordVec) {
		i = stem(i);	
	}

	std::size_t i = 0;
	while(i < queryWordVec.size() && invertedIndex.find(queryWordVec[i]) == invertedIndex.end())
		i++;

	auto firstIt = invertedIndex.find(queryWordVec[i]);
	if(firstIt == invertedIndex.end())
		return DocumentSet();

	auto curDocuments = firstIt->second;
	for(std::size_t a = i; a < queryWordVec.size(); a++)
	{
		auto it = invertedIndex.find(queryWordVec[a]);
		if(it != invertedIndex.end())
		{
			DocumentSet tmp;
			std::set_intersection(curDocuments.begin(), curDocuments.end(), it->second.begin(), it->second.end(), std::inserter(tmp, tmp.begin()));
			curDocuments = tmp;
		}
	}

	return curDocuments;
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
		for (auto docIdx : docs) {
			std::cout << articles[docIdx] << std::endl;
		}
	}

	return 0;
}
