#pragma once

#include "../../../libs/wiki_xml_dump_xerces/src/handlers/abstractWikiPageHandler.hpp"

#include <boost/container/flat_set.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <vector>

#include "../wikiArticleHandlers/vectorMutex.hpp"

class LinkExtractionHandler : public WikiXmlDumpXerces::AbstractWikiPageHandler
{
	public:
		LinkExtractionHandler(
			const std::vector<std::string>& arts, 
			const std::map<std::string, std::string>& redirs,
			std::vector<boost::container::flat_set<std::size_t>>& adjList,
			VectorMutex<1000>& vecMut
		);

		void HandleArticle(const WikiXmlDumpXerces::WikiPageData&);

	private:
		std::vector<std::string> _articles;
   		const std::map<std::string, std::string>& _redirects;
		std::vector<boost::container::flat_set<std::size_t>>& _adjList;
		VectorMutex<1000>& _vecMutex;
};
