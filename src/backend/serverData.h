#pragma once

#include <vector>
#include <string>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/subgraph.hpp>
#include <boost/graph/copy.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "../parsers/date/date.h"
#include "fullTextSearch.h"


namespace WikiMainPath {

	struct ServerData {
		public:
			using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, boost::no_property, boost::property<boost::edge_index_t,std::size_t>, boost::vecS>;
			using Subgraph = boost::subgraph<Graph>;

			ServerData(std::string input_folder);

			const std::vector<std::string> _articles;	
			const std::vector<std::string> _categories;	
			const std::vector<Date> _dates;	
			const std::vector<std::vector<std::size_t>> _category_has_article;
			const Subgraph _article_network;
			const InvertedIndex _categories_inverted_index;
			const InvertedIndex _articles_inverted_index;

		private:
			// delegated constructor
			ServerData(std::vector<std::string> arts, std::vector<std::string> cats, std::vector<Date> dates, std::vector<std::vector<std::size_t>> category_has_article, Subgraph article_network);

			static std::vector<std::string> read_articles(std::string articles_file_path);
			static std::vector<std::string> read_categories(std::string categories_file_path);
			static std::vector<Date> read_dates(std::string dates_file_path);
			static std::vector<std::vector<std::size_t>> read_category_has_article(std::string category_has_article_file_path);
			static Subgraph read_article_network(std::string article_network_path);
	};

}
