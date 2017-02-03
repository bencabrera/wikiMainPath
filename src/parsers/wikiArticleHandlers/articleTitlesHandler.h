#pragma once

#include <vector>
#include <set>
#include <map>

#include "../../../libs/wiki_xml_dump_xerces/src/handlers/abstractWikiPageHandler.hpp"
#include "../date/date.h"

class ArticleTitlesHandler : public WikiXmlDumpXerces::AbstractWikiPageHandler {
	public:
		ArticleTitlesHandler();

		void HandleArticle(const WikiXmlDumpXerces::WikiPageData&);

		std::map<std::string, Date> articles;
		std::map<std::string, std::vector<std::string>> categoriesToArticles;
		std::map<std::string, std::string> redirects;

		std::vector<std::pair<std::string, std::string>> date_but_no_key; 
		std::vector<std::pair<std::string, std::string>> key_but_no_date;

		std::size_t count() const;

	private:
		std::size_t _count;
};
