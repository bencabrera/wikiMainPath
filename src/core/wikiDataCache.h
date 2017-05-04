#pragma once

#include <vector>
#include <map>
#include <string>
#include <fstream>

#include <boost/filesystem/path.hpp>
#include <boost/container/flat_set.hpp>

#include "date.h"
#include "graph.h"

namespace WikiMainPath {

	class WikiDataCache {
		public:
			constexpr static char ARTICLES_TITLES_FILE[] = "article_titles.txt";
			constexpr static char CATEGORIES_TITLES_FILE[] = "category_titles.txt";
			constexpr static char REDIRECTS_FILE[] = "redirects.txt";
			constexpr static char ARTICLE_DATES_FILE[] = "article_dates.txt";
			constexpr static char CAT_HAS_ARTICLE_FILE[] = "category_has_article.txt";
			constexpr static char ARTICLE_NETWORK_FILE[] = "article_network.txt";
			constexpr static char CATEGORY_HIRACHY_GRAPH_FILE[] = "category_hirachy_graph.txt";

			// constructor
			WikiDataCache(std::string folder);
			WikiDataCache(
				std::shared_ptr<std::istream> article_titles_file = std::shared_ptr<std::istream>(), 
				std::shared_ptr<std::istream> article_dates_file = std::shared_ptr<std::istream>(), 
				std::shared_ptr<std::istream> category_titles_file = std::shared_ptr<std::istream>(), 
				std::shared_ptr<std::istream> redirects_file = std::shared_ptr<std::istream>(), 
				std::shared_ptr<std::istream> category_has_article_file = std::shared_ptr<std::istream>(), 
				std::shared_ptr<std::istream> article_network_file = std::shared_ptr<std::istream>(),
				std::shared_ptr<std::istream> category_hirachy_graph_file = std::shared_ptr<std::istream>()
			);

			// getter methods
			const std::vector<std::string>& article_titles();
			const std::vector<std::string>& category_titles();
			const std::map<std::string, std::string>& redirects();
			const std::vector<std::vector<Date>>& article_dates();
			const std::vector<std::vector<std::size_t>>& category_has_article();
			const std::vector<boost::container::flat_set<std::size_t>>& category_has_article_set();
			const std::vector<std::vector<std::size_t>>& article_network();
			const CategoryHirachyGraph& category_hirachy_graph();

			const std::vector<std::string>& article_titles() const;
			const std::vector<std::string>& category_titles() const;
			const std::map<std::string, std::string>& redirects() const;
			const std::vector<std::vector<Date>>& article_dates() const;
			const std::vector<std::vector<std::size_t>>& category_has_article() const;
			const std::vector<boost::container::flat_set<std::size_t>>& category_has_article_set() const;
			const std::vector<std::vector<std::size_t>>& article_network() const;
			const CategoryHirachyGraph& category_hirachy_graph() const;

			// file writer methods
			void write_article_titles(const std::map<std::string,std::vector<Date>>& articles_with_dates);
			void write_category_titles(const std::vector<std::string>& categories);
			void write_redirects(std::map<std::string,std::string> redirects);
			void write_article_dates(const std::map<std::string,std::vector<Date>>& articles_with_dates);
			void write_category_has_article(const std::vector<boost::container::flat_set<std::size_t>>& var);
			void write_article_network(const std::vector<boost::container::flat_set<std::size_t>>& adj_list);
			void write_category_hirachy_graph(const CategoryHirachyGraph& g);

		private:
			boost::filesystem::path _folder;

			std::shared_ptr<std::istream> _article_titles_file;
			std::shared_ptr<std::istream> _article_dates_file;
			std::shared_ptr<std::istream> _category_titles_file;
			std::shared_ptr<std::istream> _redirects_file;
			std::shared_ptr<std::istream> _category_has_article_file;
			std::shared_ptr<std::istream> _article_network_file;
			std::shared_ptr<std::istream> _category_hirachy_graph_file;

			// file reader methods
			std::unique_ptr<std::vector<std::string>> read_article_titles(std::string articles_file_path);
			std::unique_ptr<std::vector<std::string>> read_category_titles(std::string categories_file_path);
			std::unique_ptr<std::map<std::string,std::string>> read_redirects(std::string categories_file_path);
			std::unique_ptr<std::vector<std::vector<Date>>> read_article_dates(std::string dates_file_path);
			std::unique_ptr<std::vector<std::vector<std::size_t>>> read_category_has_article(std::string category_has_article_file_path);
			std::unique_ptr<std::vector<boost::container::flat_set<std::size_t>>> read_category_has_article_set(std::string category_has_article_file_path);
			std::unique_ptr<std::vector<std::vector<std::size_t>>> read_article_network(std::string article_network_path);
			std::unique_ptr<CategoryHirachyGraph> read_category_hirachy_graph(std::string path);

			std::unique_ptr<std::vector<std::string>> _article_titles;
			std::unique_ptr<std::vector<std::string>> _category_titles;
			std::unique_ptr<std::map<std::string, std::string>> _redirects;
			std::unique_ptr<std::vector<std::vector<Date>>> _article_dates;
			std::unique_ptr<std::vector<std::vector<std::size_t>>> _category_has_article;
			std::unique_ptr<std::vector<boost::container::flat_set<std::size_t>>> _category_has_article_set;
			std::unique_ptr<std::vector<std::vector<std::size_t>>> _article_network;
			std::unique_ptr<CategoryHirachyGraph> _category_hirachy_graph;
	};

}
