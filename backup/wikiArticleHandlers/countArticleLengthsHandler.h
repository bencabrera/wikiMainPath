#pragma once

#include <vector>
#include <map>

#include "../../../libs/wiki_xml_dump_xerces/src/handlers/abstractWikiPageHandler.hpp"

class CountArticleLengthHandler : public WikiXmlDumpXerces::AbstractWikiPageHandler {
	public:
		using ArticleProperties = std::tuple<std::size_t, std::size_t, std::size_t>;


		CountArticleLengthHandler(std::vector<std::map<std::string, std::vector<std::string>>>& exi);

		void HandleArticle(const WikiXmlDumpXerces::WikiPageData&);

	private: 
		std::vector<std::map<std::string, std::vector<std::string>>>& existing_results;

		static void clean_and_encode_title(std::string& title);
		static std::string url_encode(const std::string &value);
		static std::string preprocess(const std::string& input);
		static ArticleProperties compute_properties(const std::string& input);
};
