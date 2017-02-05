#include "serverData.h"

#include <fstream>

#include "../parsers/fileNames.h"

namespace WikiMainPath {

	ServerData::ServerData(std::string input_folder)
		:ServerData(
			read_articles(input_folder + ARTICLES_FILE),
			read_categories(input_folder + CATEGORIES_FILE),
			read_dates(input_folder + ARTICLE_DATES_FILE),
			read_category_has_article(input_folder + CAT_HAS_ARTICLE_FILE),
			read_article_network(input_folder + ARTICLE_NETWORK_FILE)
		)
	{}

	// delegated constructor
	ServerData::ServerData(std::vector<std::string> arts, std::vector<std::string> cats, std::vector<Date> dates, std::vector<std::vector<std::size_t>> category_has_article, Subgraph article_network)
		:_articles(arts),
		_categories(cats),
		_dates(dates),
		_category_has_article(category_has_article),
		_categories_inverted_index(buildInvertedIndex(_categories)),
		_articles_inverted_index(buildInvertedIndex(_articles))
	{}

	std::vector<std::string> ServerData::read_articles(std::string articles_file_path)
	{
		std::ifstream articles_file(articles_file_path);	
		if(!articles_file.is_open())
			return std::vector<std::string>();

		std::vector<std::string> rtn;

		std::string line;
		while(std::getline(articles_file, line))
			rtn.push_back(line);

		return rtn;
	}

	std::vector<std::string> ServerData::read_categories(std::string categories_file_path)
	{
		std::ifstream categories_file(categories_file_path);	
		if(!categories_file.is_open())
			return std::vector<std::string>();

		std::vector<std::string> rtn;

		std::string line;
		while(std::getline(categories_file, line))
			rtn.push_back(line);

		return rtn;
	}

	std::vector<Date> ServerData::read_dates(std::string dates_file_path)
	{
		std::ifstream dates_file(dates_file_path);	
		if(!dates_file.is_open())
			return std::vector<Date>();

		std::vector<Date> rtn;

		std::string line;
		while(std::getline(dates_file, line))
			rtn.push_back(Date::deserialize(line));

		return rtn;
	}

	std::vector<std::vector<std::size_t>> ServerData::read_category_has_article(std::string category_has_article_file_path)
	{
		std::ifstream category_has_article_file(category_has_article_file_path);	
		if(!category_has_article_file.is_open())
			return std::vector<std::vector<std::size_t>>();

		std::string line;
		std::vector<std::vector<std::size_t>> category_has_article;
		while(std::getline(category_has_article_file, line))
		{
			std::vector<std::size_t> linked_articles;	

			std::istringstream ss(line);
			while(!ss.eof())
			{
				std::string tmpStr;
				ss >> tmpStr;
				if(tmpStr != "")
					linked_articles.push_back(std::stoi(tmpStr));
			}

			category_has_article.push_back(linked_articles);
		}

		return category_has_article;
	}

	ServerData::Subgraph ServerData::read_article_network(std::string article_network_path)
	{
		std::ifstream adj_list_file(article_network_path);	
		if(!adj_list_file.is_open())
			return Subgraph();

		Subgraph graph;
		std::size_t source = 0;
		std::string line;
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

		return graph;
	}
}
