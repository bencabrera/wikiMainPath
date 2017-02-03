#pragma once

#include <vector>
#include <set>
#include <map>

#include "../../../libs/wiki_xml_dump_xerces/src/handlers/abstractWikiPageHandler.hpp"
#include "../date/date.h"

class ArticleDatesAndCategoriesHandler : public WikiXmlDumpXerces::AbstractWikiPageHandler {
	public:
		ArticleDatesAndCategoriesHandler();

		void HandleArticle(const WikiXmlDumpXerces::WikiPageData&);

		std::map<std::string, Date> articles;
		std::vector<std::string> categories;
		std::map<std::string, std::string> redirects;

		std::vector<std::pair<std::string, std::string>> date_but_no_key; 
		std::vector<std::pair<std::string, std::string>> key_but_no_date;

		bool ExtractOnlyArticlesWithDates;
};
