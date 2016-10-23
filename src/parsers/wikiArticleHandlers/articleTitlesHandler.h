#pragma once

#include <vector>
#include <set>
#include <map>

#include "../xercesHandlers/abstractArticleHandler.h"
#include "../articleNetwork/dateExtractor.h"
#include "../articleNetwork/date.h"

class ArticleTitlesHandler : public AbstractArticleHandler{
	public:
		ArticleTitlesHandler();

		void HandleArticle(const ArticleData&);

		std::map<std::string, Date> articles;
		std::map<std::string, std::vector<std::string>> categoriesToArticles;
		std::map<std::string, std::string> redirects;

		std::size_t count() const;

	private:
		DateExtractor extractDate;
		std::size_t _count;
};
