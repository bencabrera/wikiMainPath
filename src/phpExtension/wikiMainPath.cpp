#include <phpcpp.h>

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/algorithm/string/regex.hpp>

#include "../program/fullTextSearch.h"

InvertedIndex invertedCategoryIndex;
std::vector<std::string> articles;

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property, boost::edge_index_t, boost::vecS> Graph;

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
		std::string pathToCategories= "/home/cabrera/UCSM/wikiMainPath/bin/debug/out/categories.txt";


		const fs::path inputArticlePath(pathToArticles);
		if(!fs::exists(inputArticlePath))
		{
			Php::error << "The path " << inputArticlePath << " specified for wikiMainPath.path_to_articles does not exist." << std::endl;
			return extension;
		}

		const fs::path inputCategoriesPath(pathToCategories);
		if(!fs::exists(inputCategoriesPath))
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

		std::ifstream categories_file(inputCategoriesPath.string());	
		if(!categories_file.is_open())
		{
			Php::error << "Could not open file specified in path " << inputCategoriesPath << " specified for wikiMainPath.path_to_articles." << std::endl;
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

		// read in categories file
		std::vector<std::string> categories;
		std::vector<std::set<std::size_t>> category_refs;

		while(std::getline(categories_file, line))
		{
			std::istringstream ss(line);
			std::string title, refsStr;
			std::getline(ss, title, '\t');
			std::getline(ss, refsStr, '\t');
			categories.push_back(title);

			//std::vector<std::string> refs;
			//boost::split_regex(refs, refsStr, boost::regex(";-;"));

			//std::set<std::size_t> refs_idx;
			//std::size_t	ref_idx;
			//for (auto r : refs) 
			//	if(getPosition(articles, r, ref_idx))
			//		refs_idx.insert(ref_idx);	

			//category_refs.push_back(refs_idx);
		}

		invertedCategoryIndex = buildInvertedIndex(articles);

		extension.add<queryCategories>("queryCategories");


		return extension;
	}
}
