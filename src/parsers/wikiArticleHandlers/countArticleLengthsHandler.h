#pragma once

#include <vector>
#include <set>
#include <map>

#include "../xercesHandlers/abstractArticleHandler.h"
#include "../date/date.h"

class CountArticleLengthHandler : public AbstractArticleHandler{
	public:
		using ArticleProperties = std::tuple<std::size_t, std::size_t, std::size_t>;

		void HandleArticle(const ArticleData&);

		static std::string preprocess(const std::string& input);
		static ArticleProperties compute_properties(const std::string& input);

		std::map<std::string, ArticleProperties> article_counts;
};
