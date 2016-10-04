#pragma once

#include "../xml/abstractArticleHandler.h"
#include "../articleNetwork/linkListExtractor.h"

#include <boost/container/flat_set.hpp>
#include <vector>

#include "../articleNetwork/articleGraph.h"
#include "../articleNetwork/date.h"
#include "../wikiHandlers/vectorMutex.hpp"

class LinkExtractionHandler : public AbstractArticleHandler
{
	public:
		LinkExtractionHandler(
			const std::vector<std::string>& arts, 
			const std::vector<Date>& dates, 
			const std::map<std::string, std::string>& redirs,
			std::vector<boost::container::flat_set<std::size_t>>& adjList,
			VectorMutex<1000>& vecMut
		);

		void HandleArticle(const ArticleData&);

	private:
		bool GetPosition(const std::string& title, std::size_t& v) const;

		std::vector<std::string> _articles;
		std::vector<Date> _dates;
   		const std::map<std::string, std::string>& _redirects;
		std::vector<boost::container::flat_set<std::size_t>>& _adjList;
		VectorMutex<1000>& _vecMutex;
};
