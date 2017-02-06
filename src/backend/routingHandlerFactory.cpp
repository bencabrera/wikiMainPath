#include "routingHandlerFactory.h"

#include <iostream>
#include <Poco/URI.h>

#include "httpRequestHandlers/mpaHandler.h"
#include "httpRequestHandlers/searchHandler.h"
#include "httpRequestHandlers/articleListHandler.h"
#include "httpRequestHandlers/articleNetworkHandler.h"
#include "httpRequestHandlers/notFoundHandler.h"

namespace WikiMainPath {

	RoutingHandlerFactory::RoutingHandlerFactory(ServerData& server_data) 
		:_server_data(server_data)
	{}

	Poco::Net::HTTPRequestHandler* RoutingHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest &request)
	{
		Poco::URI uri(request.getURI());

		if(uri.getPath() == "/search-category")
			return new SearchHandler("maching-categories", _server_data._categories, _server_data._categories_inverted_index);

		if(uri.getPath() == "/search-article")
			return new SearchHandler("maching-articles", _server_data._articles, _server_data._articles_inverted_index);

		if(uri.getPath() == "/article-list-in-category")
			return new ArticleListHandler(_server_data._articles, _server_data._category_has_article);

		if(uri.getPath() == "/article-network-in-category")
			return new ArticleNetworkHandler(_server_data._articles, _server_data._category_has_article, _server_data._article_network);

		if(uri.getPath() == "/mpa-in-category")
			return new MpaHandler(_server_data._articles, _server_data._category_has_article, _server_data._article_network);

		return new NotFoundHandler();
	} 

}
