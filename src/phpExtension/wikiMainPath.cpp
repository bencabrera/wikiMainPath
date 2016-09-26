#include <phpcpp.h>

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include <boost/filesystem.hpp>

#include "../program/fullTextSearch.h"

InvertedIndex invertedCategoryIndex;
std::vector<std::string> articles;

Php::Value queryCategories(Php::Parameters &params)
{ 
	Php::Value queryVal = params[0];
	std::string queryStr = queryVal;

	Php::Value rtn;

	auto docs = query(invertedCategoryIndex, queryStr);

	std::size_t i = 0;
	for (auto docIdx : docs) {
		rtn[i++] = articles[docIdx];
	}

	return rtn;
}

extern "C" {
	namespace fs = boost::filesystem;

	//const std::string pathToArticles = "/home/cabrera/UCSM/wikiMainPath/bin/debug/out";
	//
	

	PHPCPP_EXPORT void *get_module() 
	{
		static Php::Extension extension("wikiMainPath", "1.0");

		extension.add(Php::Ini("wikiMainPath.path_to_articles", "hallo"));
		extension.add(Php::Ini("wikiMainPath.path_to_categories", ""));


		//std::string pathToArticles = Php::ini_get("wikiMainPath.path_to_articles");
		std::string pathToArticles = "/home/cabrera/UCSM/wikiMainPath/bin/debug/out/articles_with_dates.txt";


		const fs::path inputArticlePath(pathToArticles);
		if(!fs::exists(inputArticlePath))
		{
			Php::error << "The path " << inputArticlePath << " specified for wikiMainPath.path_to_articles does not exist." << std::endl;
			return extension;
		}

		std::ifstream articles_file(inputArticlePath.string());	
		if(!articles_file.is_open())
		{
			Php::error << "Could not open file specified in path " << inputArticlePath << " specified for wikiMainPath.path_to_articles." << std::endl;
			return extension;
		}

		std::string line;
		while(std::getline(articles_file, line))
		{
			std::istringstream ss(line);
			std::string title, dateStr;
			std::getline(ss, title, '\t');
			std::getline(ss, dateStr, '\t');
			articles.push_back(title);
		}

		invertedCategoryIndex = buildInvertedIndex(articles);

		extension.add<queryCategories>("queryCategories");


		return extension;
	}
}
