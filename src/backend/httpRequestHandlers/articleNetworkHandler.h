#pragma once

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include <boost/container/flat_set.hpp>
#include "../../core/graph.h"

namespace WikiMainPath {

	class ArticleNetworkHandler: public Poco::Net::HTTPRequestHandler
	{
		public:
			ArticleNetworkHandler(const std::vector<std::string>& articles, const std::vector<boost::container::flat_set<std::size_t>>& category_has_article, ArticleNetwork& article_network);

			void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);

		private:
			const std::vector<std::string>& _articles; 
			const std::vector<boost::container::flat_set<std::size_t>>& _category_has_article;
			ArticleNetwork& _article_network;
	};

}
