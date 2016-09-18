#pragma once

#include <vector>
#include <set>
#include <map>

#include "../xml/abstractArticleHandler.h"
#include "../articleNetwork/dateExtractor.h"

class ArticleTitlesHandler : public AbstractArticleHandler{
	public:
		void HandleArticle(const ArticleData&);

		std::map<std::string, std::tm> articles;
		std::map<std::string, std::vector<std::string>> categoriesToArticles;
		std::map<std::string, std::string> redirects;

	private:
		DateExtractor extractDate;
};
