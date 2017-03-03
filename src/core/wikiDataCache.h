#pragma once

#include <vector>
#include <map>
#include <string>

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
			constexpr static char EVENT_TITLES_FILE[] = "event_titles.txt";
			constexpr static char CAT_HAS_EVENT_FILE[] = "category_has_event.txt";
			constexpr static char EVENT_NETWORK_FILE[] = "event_network.txt";

			// constructor
			WikiDataCache(std::string folder);

			// getter methods
			const std::vector<std::string>& article_titles();
			const std::vector<std::string>& category_titles();
			const std::map<std::string, std::string>& redirects();
			const std::vector<std::vector<Date>>& article_dates();
			const std::vector<std::vector<std::size_t>>& category_has_article();
			const std::vector<boost::container::flat_set<std::size_t>>& category_has_article_set();
			const ArticleNetwork& article_network();
			const std::vector<std::string>& event_titles();
			const std::vector<std::vector<std::size_t>>& category_has_event();
			const EventNetwork& event_network();

			// file writer methods
			void write_article_titles(const std::map<std::string,std::vector<Date>>& articles_with_dates);
			void write_category_titles(const std::vector<std::string>& categories);
			void write_redirects(std::map<std::string,std::string> redirects);
			void write_article_dates(const std::map<std::string,std::vector<Date>>& articles_with_dates);
			void write_category_has_article(const std::vector<boost::container::flat_set<std::size_t>>& var);
			void write_article_network(const std::vector<boost::container::flat_set<std::size_t>>& adj_list);
			void write_event_titles(const std::vector<std::string>& event_titles);
			void write_category_has_event(const std::vector<std::vector<std::size_t>>& cat_has_art);
			void write_event_network(const std::vector<boost::container::flat_set<std::size_t>>& adj_list);

		private:
			boost::filesystem::path _folder;

			// file reader methods
			static std::unique_ptr<std::vector<std::string>> read_article_titles(std::string articles_file_path);
			static std::unique_ptr<std::vector<std::string>> read_category_titles(std::string categories_file_path);
			static std::unique_ptr<std::map<std::string,std::string>> read_redirects(std::string categories_file_path);
			static std::unique_ptr<std::vector<std::vector<Date>>> read_article_dates(std::string dates_file_path);
			static std::unique_ptr<std::vector<std::vector<std::size_t>>> read_category_has_article(std::string category_has_article_file_path);
			static std::unique_ptr<std::vector<boost::container::flat_set<std::size_t>>> read_category_has_article_set(std::string category_has_article_file_path);
			static std::unique_ptr<ArticleNetwork> read_article_network(std::string article_network_path);

			static std::unique_ptr<std::vector<std::string>> read_event_titles(std::string articles_file_path);
			static std::unique_ptr<std::vector<std::vector<std::size_t>>> read_category_has_event(std::string category_has_article_file_path);
			static std::unique_ptr<ArticleNetwork> read_event_network(std::string article_network_path);

			std::unique_ptr<std::vector<std::string>> _article_titles;
			std::unique_ptr<std::vector<std::string>> _category_titles;
			std::unique_ptr<std::map<std::string, std::string>> _redirects;
			std::unique_ptr<std::vector<std::vector<Date>>> _article_dates;
			std::unique_ptr<std::vector<std::vector<std::size_t>>> _category_has_article;
			std::unique_ptr<std::vector<boost::container::flat_set<std::size_t>>> _category_has_article_set;
			std::unique_ptr<ArticleNetwork> _article_network;

			std::unique_ptr<std::vector<std::string>> _event_titles;
			std::unique_ptr<std::vector<std::vector<std::size_t>>> _category_has_event;
			std::unique_ptr<EventNetwork> _event_network;
	};

}
