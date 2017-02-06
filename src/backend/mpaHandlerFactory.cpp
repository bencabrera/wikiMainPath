#include "mpaHandlerFactory.h"

#include <iostream>
#include <Poco/URI.h>

#include "httpRequestHandlers/mpaRequestHandler.h"
#include "httpRequestHandlers/searchCategoryHandler.h"
#include "httpRequestHandlers/notFoundHandler.h"

namespace WikiMainPath {

	MpaHandlerFactory::MpaHandlerFactory(const ServerData& server_data) 
		:_server_data(server_data)
	{}

	Poco::Net::HTTPRequestHandler* MpaHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest &request)
	{
		Poco::URI uri(request.getURI());

		if(uri.getPath() == "/search-category")
			return new SearchCategoryHandler(_server_data._categories, _server_data._categories_inverted_index);

		// if(request.getURI() == "/search-article")
		// return new SearchArticleHandler();

		// if(request.getURI() == "/article-network-in-category")
		// return new ArticleNetworkHandler();

		// if(request.getURI() == "/main-path-in-category")
		// return new MainPathHandler();

		// if(request.getURI() == "/mpa-in-category")
		// return new MpaHandler();

		return new NotFoundHandler();
	} 

}
