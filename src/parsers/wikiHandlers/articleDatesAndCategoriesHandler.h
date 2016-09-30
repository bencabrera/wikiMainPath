#pragma once

#include <vector>
#include <set>
#include <map>

#include "../xml/abstractArticleHandler.h"
#include "../articleNetwork/dateExtractor.h"
#include "../articleNetwork/date.h"

class ArticleDatesAndCategoriesHandler : public AbstractArticleHandler{
	public:
		void HandleArticle(const ArticleData&);

		std::map<std::string, Date> articles;
		std::vector<std::string> categories;
		std::map<std::string, std::string> redirects;

	private:
		DateExtractor extractDate;
};
