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

		void HandleArticle(const ArticleData&);

	private: 
		std::vector<std::map<std::string, std::vector<std::string>>>& existing_results;

		static void clean_and_encode_title(std::string& title);
		static std::string url_encode(const std::string &value);
		static std::string preprocess(const std::string& input);
		static ArticleProperties compute_properties(const std::string& input);
};
