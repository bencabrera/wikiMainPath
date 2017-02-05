#include "mpaHandlerFactory.h"

#include <iostream>

#include "mpaRequestHandler.h"
#include "searchCategoryHandler.h"
#include "notFoundHandler.h"

namespace WikiMainPath {

	MpaHandlerFactory::MpaHandlerFactory(const ServerData& server_data) 
		:_server_data(server_data)
	{}

	Poco::Net::HTTPRequestHandler* MpaHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest &request)
	{
		if(request.getURI() == "/search-category")
			return new SearchCategoryHandler(_server_data._categories_inverted_index);

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
