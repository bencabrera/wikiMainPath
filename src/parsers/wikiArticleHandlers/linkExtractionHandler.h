#pragma once

#include "../xercesHandlers/abstractArticleHandler.h"

#include <boost/container/flat_set.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <vector>

#include "../date/date.h"
#include "../wikiArticleHandlers/vectorMutex.hpp"

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

		std::function<bool(std::size_t, std::size_t)> OrderCallback;

	private:
		bool GetPosition(const std::string& title, std::size_t& v) const;

		std::vector<std::string> _articles;
		std::vector<Date> _dates;
   		const std::map<std::string, std::string>& _redirects;
		std::vector<boost::container::flat_set<std::size_t>>& _adjList;
		VectorMutex<1000>& _vecMutex;
};
