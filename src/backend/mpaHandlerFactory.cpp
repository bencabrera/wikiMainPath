#include "mpaHandlerFactory.h"

#include <iostream>

#include "mpaRequestHandler.h"
#include "searchCategoryHandler.h"
#include "notFoundHandler.h"

MpaHandlerFactory::MpaHandlerFactory() 
{
}

Poco::Net::HTTPRequestHandler* MpaHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest &request)
{
	if(request.getURI() == "/search-category")
		return new SearchCategoryHandler();

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
