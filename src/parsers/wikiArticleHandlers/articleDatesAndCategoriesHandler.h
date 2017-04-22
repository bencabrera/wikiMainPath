#pragma once

#include <vector>
#include <set>
#include <map>
#include <mutex>

#include "../../../libs/wiki_xml_dump_xerces/src/handlers/abstractWikiPageHandler.hpp"
#include "../../core/date.h"

class ArticleDatesAndCategoriesHandler : public WikiXmlDumpXerces::AbstractWikiPageHandler {
	public:
		static constexpr std::size_t WRITE_INTERVAL = 10000;

		ArticleDatesAndCategoriesHandler(
			std::ostream& article_titles_file,
			std::ostream& article_dates_file,
			std::ostream& category_titles_file,
			std::ostream& redirect_file,
			std::mutex& _article_titles_mutex,
			std::mutex& _category_titles_mutex,
			std::mutex& _redirect_mutex,
			std::ostream* report_ostr = nullptr
		);

		~ArticleDatesAndCategoriesHandler();

		void HandleArticle(const WikiXmlDumpXerces::WikiPageData&);


		bool ExtractOnlyArticlesWithDates;

		std::size_t n_errors;

	private:
		std::ostream* _report_ostr;

		void write_article_titles();
		void write_category_titles();
		void write_redirects();

		std::map<std::string, std::vector<Date>> _articles;
		std::vector<std::string> _categories;
		std::map<std::string, std::string> _redirects;

		std::ostream& _article_titles_file;
		std::ostream& _article_dates_file;
		std::ostream& _category_titles_file;
		std::ostream& _redirect_file;

		std::mutex& _article_titles_mutex;
		std::mutex& _category_titles_mutex;
		std::mutex& _redirect_mutex;
};
