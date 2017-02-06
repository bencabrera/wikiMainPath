#include "routingHandlerFactory.h"

#include <iostream>
#include <Poco/URI.h>

#include "httpRequestHandlers/mpaRequestHandler.h"
#include "httpRequestHandlers/searchHandler.h"
#include "httpRequestHandlers/notFoundHandler.h"

namespace WikiMainPath {

	RoutingHandlerFactory::RoutingHandlerFactory(const ServerData& server_data) 
		:_server_data(server_data)
	{}

	Poco::Net::HTTPRequestHandler* RoutingHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest &request)
	{
		Poco::URI uri(request.getURI());

		if(uri.getPath() == "/search-category")
			return new SearchHandler("maching-categories", _server_data._categories, _server_data._categories_inverted_index);

		if(uri.getPath() == "/search-article")
			return new SearchHandler("maching-articles", _server_data._articles, _server_data._articles_inverted_index);

		// if(request.getURI() == "/article-network-in-category")
		// return new ArticleNetworkHandler();

		// if(request.getURI() == "/main-path-in-category")
		// return new MainPathHandler();

		// if(request.getURI() == "/mpa-in-category")
		// return new MpaHandler();

		return new NotFoundHandler();
	} 

}
