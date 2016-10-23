#include <phpcpp.h>

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/subgraph.hpp>
#include <boost/algorithm/string/regex.hpp>

#include "../program/fullTextSearch.h"
#include "../parsers/articleNetwork/dateExtractor.h"
#include "../program/readDataFromFile.h"

InvertedIndex invertedCategoryIndex;
std::vector<std::string> articles;
std::vector<Date> dates;
std::vector<std::string> categories;
std::vector<std::vector<std::size_t>> category_has_article;

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, boost::no_property, boost::property<boost::edge_index_t,std::size_t>, boost::vecS> Graph;
typedef boost::subgraph<Graph> Subgraph;

Graph readGraph(std::istream& istr)
{
	std::size_t numVertices, numEdges;
	istr >> numVertices >> numEdges;

	Graph g(numVertices);

	while(!istr.eof())
	{
		std::size_t source, target;
		istr >> source >> target;
		boost::add_edge(source, target, g);
	}	

	return g;
}

bool getPosition(const std::vector<std::string>& vec, const std::string& str, std::size_t& v) 
{
	auto it = std::lower_bound(vec.begin(), vec.end(), str);
	if(it != vec.end() && *it == str)
	{
		v = it-vec.begin();
		return true;
	}	
	else
		return false;
}

//Subgraph getSubgraphOfCategory(Graph& g, std::string 

Php::Value queryCategories(Php::Parameters &params)
{ 
	Php::Value queryVal = params[0];
	std::string queryStr = queryVal;

	Php::Value rtn;

	auto docs = query(invertedCategoryIndex, queryStr);

	std::size_t i = 0;
	for (auto docIdx : docs) {
		rtn[i++] = categories[docIdx];
	}

	return rtn;
}

Php::Value listArticlesInCategory(Php::Parameters& params)
{
	Php::Value category_title_param = params[0];
	std::string category_title = category_title_param;
	boost::trim(category_title);

	Php::Value rtn;

	auto it = std::lower_bound(categories.begin(), categories.end(), category_title);
	if(it != categories.end() && *it == category_title)
	{
		std::size_t category_idx = it - categories.begin();
		for (std::size_t i = 0; i < category_has_article[category_idx].size(); i++) {
			rtn[i] = articles[category_has_article.at(category_idx).at(i)];
		}

		return rtn;
	}
	else
		return false;
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
		const std::string pathToArticles = "/home/cabrera/UCSM/wikiMainPath/bin/release/out/articles_with_dates.txt";
		const std::string pathToCategories = "/home/cabrera/UCSM/wikiMainPath/bin/release/out/categories.txt";
		const std::string pathToCategoryHasArticle = "/home/cabrera/UCSM/wikiMainPath/bin/release/out/category_has_article.txt";
		const std::string pathToAdjList = "/home/cabrera/UCSM/wikiMainPath/bin/release/out/adjList.txt";

		const fs::path inputArticlePath(pathToArticles);
		if(!fs::exists(inputArticlePath))
		{
			Php::error << "The path " << inputArticlePath << " specified for wikiMainPath.path_to_articles does not exist." << std::endl;
			return extension;
		}

		const fs::path inputCategoriesPath(pathToCategories);
		if(!fs::exists(inputCategoriesPath))
		{
			Php::error << "The path " << inputCategoriesPath << " specified for wikiMainPath.path_to_categories does not exist." << std::endl;
			return extension;
		}

		const fs::path inputCategoryHasArticlePath(pathToCategoryHasArticle);
		if(!fs::exists(inputCategoryHasArticlePath))
		{
			Php::error << "The path " << inputCategoryHasArticlePath << " specified for wikiMainPath.path_to_category_has_article does not exist." << std::endl;
			return extension;
		}

		const fs::path inputAdjListPath(pathToAdjList);
		if(!fs::exists(inputAdjListPath))
		{
			Php::error << "The path " << inputAdjListPath << " specified for wikiMainPath.path_to_adj_list does not exist." << std::endl;
			return extension;
		}

		std::ifstream articles_file(inputArticlePath.string());	
		if(!articles_file.is_open())
		{
			Php::error << "Could not open file specified in path " << inputArticlePath << " specified for wikiMainPath.path_to_articles." << std::endl;
			return extension;
		}

		std::ifstream categories_file(inputCategoriesPath.string());	
		if(!categories_file.is_open())
		{
			Php::error << "Could not open file specified in path " << inputCategoriesPath << " specified for wikiMainPath.path_to_categories." << std::endl;
			return extension;
		}

		std::ifstream category_has_article_file(inputCategoryHasArticlePath.string());	
		if(!category_has_article_file.is_open())
		{
			Php::error << "Could not open file specified in path " << inputCategoryHasArticlePath << " specified for wikiMainPath.path_to_category_has_article." << std::endl;
			return extension;
		}

		std::ifstream adj_list_file(inputAdjListPath.string());	
		if(!adj_list_file.is_open())
		{
			Php::error << "Could not open file specified in path " << inputAdjListPath << " specified for wikiMainPath.path_to_adj_list." << std::endl;
			return extension;
		}

		std::string line;
		std::size_t count = 0;
		while(std::getline(articles_file, line))
		{
			std::istringstream ss(line);
			std::string title, dateStr;
			std::getline(ss, title, '\t');
			std::getline(ss, dateStr, '\t');
			articles.push_back(title);
			dates.push_back(Date::deserialize(dateStr));
			count++;
		}

		// read in categories file
		while(std::getline(categories_file, line))
		{
			boost::trim(line);
			categories.push_back(line);
		}

		category_has_article.reserve(categories.size());
		while(std::getline(category_has_article_file, line))
		{
			std::vector<std::size_t> linked_articles;	

			std::istringstream ss(line);
			while(!ss.eof())
			{
				std::string tmpStr;
				ss >> tmpStr;
				boost::trim(tmpStr);
				if(tmpStr != "")
					linked_articles.push_back(std::stoi(tmpStr));
			}

			category_has_article.push_back(linked_articles);
		}

		Subgraph graph(articles.size());
		std::size_t source = 0;
		while(std::getline(adj_list_file, line))
		{
			std::istringstream ss(line);
			while(!ss.eof())
			{
				std::string tmpStr;
				ss >> tmpStr;
				boost::trim(tmpStr);
				if(tmpStr != "")
				{
					std::size_t target = std::stoi(tmpStr);
					boost::add_edge(source, target, graph);
				}
			}

			source++;
		}

		invertedCategoryIndex = buildInvertedIndex(categories);

		extension.add<queryCategories>("query_categories");
		extension.add<listArticlesInCategory>("list_articles_in_category");

		return extension;
	}
}
