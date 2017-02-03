#include "readDataFromFile.h"

Subgraph readDataFromFile (
		const boost::filesystem::path& inputFolder, 
		std::vector<std::string>& articles, 
		std::vector<Date>& dates, 
		std::vector<std::string>& categories, 
		std::vector<std::vector<std::size_t>>& category_has_article
)
{
	std::ifstream articles_file((inputFolder / "articles_with_dates.txt").string());	
	std::ifstream categories_file((inputFolder / "categories.txt").string());	
	std::ifstream category_has_article_file((inputFolder / "category_has_article.txt").string());	
	std::ifstream adj_list_file((inputFolder / "adjList.txt").string());	

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

	return graph;
}


