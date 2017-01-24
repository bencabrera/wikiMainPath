#pragma once

#include <vector>
#include <set>
#include <map>

#include "../xercesHandlers/abstractArticleHandler.h"
#include "../date/date.h"

class ArticleDatesAndCategoriesHandler : public AbstractArticleHandler{
	public:
		ArticleDatesAndCategoriesHandler();

		void HandleArticle(const ArticleData&);

		std::map<std::string, Date> articles;
		std::vector<std::string> categories;
		std::map<std::string, std::string> redirects;

		std::vector<std::pair<std::string, std::string>> date_but_no_key; 
		std::vector<std::pair<std::string, std::string>> key_but_no_date;

		bool ExtractOnlyArticlesWithDates;
};
