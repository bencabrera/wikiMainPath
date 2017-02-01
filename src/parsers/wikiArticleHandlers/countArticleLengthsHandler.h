#pragma once

#include <vector>
#include <set>
#include <map>

#include "../xercesHandlers/abstractArticleHandler.h"
#include "../date/date.h"

class CountArticleLengthHandler : public AbstractArticleHandler{
	public:
		using ArticleProperties = std::tuple<std::size_t, std::size_t, std::size_t>;


		CountArticleLengthHandler(std::vector<std::map<std::string, std::vector<std::string>>>& exi);

		void clean_and_encode_title(std::string& title);

		void HandleArticle(const ArticleData&);

		static std::string preprocess(const std::string& input);
		static ArticleProperties compute_properties(const std::string& input);

	private: 
		std::vector<std::map<std::string, std::vector<std::string>>>& existing_results;
};
