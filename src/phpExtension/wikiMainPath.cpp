#include <phpcpp.h>

#include <iostream>
//#include <string>
//#include <sstream>
//#include <fstream>
//
//#include <boost/filesystem.hpp>

extern "C" {
	//namespace fs = boost::filesystem;

	//const std::string pathToArticles = "/home/cabrera/UCSM/wikiMainPath/bin/debug/out";

	void queryCategories()
	{ 
		Php::out << "Hallo Welt!" << std::endl;

		//std::string pathToArticles = Php::ini_get("path_to_articles");
		//Php::out << pathToArticles << std::endl;
	}

	PHPCPP_EXPORT void *get_module() 
	{
		static Php::Extension extension("wikiMainPath", "1.0");


////		//const fs::path inputArticleFolder(pathToArticles);
//		std::vector<std::string> articles;
//
//		std::ifstream articles_file((inputArticleFolder / "articles_with_dates.txt").string());	
//
//		std::string line;
//		while(std::getline(articles_file, line))
//		{
//			std::istringstream ss(line);
//			std::string title, dateStr;
//			std::getline(ss, title, '\t');
//			std::getline(ss, dateStr, '\t');
//			articles.push_back(title);
//		}
		extension.add<queryCategories>("queryCategories");

		return extension;
	}
}
