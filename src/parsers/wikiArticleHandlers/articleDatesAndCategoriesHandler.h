#pragma once

#include <vector>
#include <set>
#include <map>

#include "../xercesHandlers/abstractArticleHandler.h"
#include "../date/dateExtractor.h"
#include "../date/date.h"

class ArticleDatesAndCategoriesHandler : public AbstractArticleHandler{
	public:
		ArticleDatesAndCategoriesHandler();

		void HandleArticle(const ArticleData&);

		std::map<std::string, Date> articles;
		std::vector<std::string> categories;
		std::map<std::string, std::string> redirects;

		bool ExtractOnlyArticlesWithDates;

	private:
		DateExtractor extractDate;
};
