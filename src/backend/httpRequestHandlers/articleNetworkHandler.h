#pragma once

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include "../serverData.h"

namespace WikiMainPath {

	class ArticleNetworkHandler: public Poco::Net::HTTPRequestHandler
	{
		public:
			using Graph = ServerData::Subgraph;

			ArticleNetworkHandler(const std::vector<std::string>& articles, const std::vector<std::vector<std::size_t>>& category_has_article, Graph& article_network);

			void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);

		private:
			const std::vector<std::string>& _articles; 
			const std::vector<std::vector<std::size_t>>& _category_has_article;
			Graph& _article_network;
	};

}
