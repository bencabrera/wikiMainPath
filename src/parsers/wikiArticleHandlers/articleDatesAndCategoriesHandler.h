#pragma once

#include <vector>
#include <set>
#include <map>

#include "../../../libs/wiki_xml_dump_xerces/src/handlers/abstractWikiPageHandler.hpp"
#include "../../core/date.h"

class ArticleDatesAndCategoriesHandler : public WikiXmlDumpXerces::AbstractWikiPageHandler {
	public:
		ArticleDatesAndCategoriesHandler(std::ostream* report_ostr = nullptr);

		void HandleArticle(const WikiXmlDumpXerces::WikiPageData&);

		std::map<std::string, std::vector<Date>> articles;
		std::vector<std::string> categories;
		std::map<std::string, std::string> redirects;

		bool ExtractOnlyArticlesWithDates;

		std::size_t n_errors;

	private:
		std::ostream* _report_ostr;
};
