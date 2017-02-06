#pragma once

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include "../fullTextSearch.h"

namespace WikiMainPath {

	class ArticleListHandler: public Poco::Net::HTTPRequestHandler
	{
		public:
			ArticleListHandler(const std::vector<std::string>& articles, const std::vector<std::vector<std::size_t>>& category_has_article);

			void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);

		private:
			const std::vector<std::string>& _articles; 
			const std::vector<std::vector<std::size_t>>& _category_has_article;
	};

}
