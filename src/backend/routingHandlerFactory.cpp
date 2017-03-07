#include "routingHandlerFactory.h"

#include <iostream>
#include <Poco/URI.h>

// #include "httpRequestHandlers/mpaHandler.h"
#include "httpRequestHandlers/searchHandler.h"
#include "httpRequestHandlers/articleListHandler.h"
// #include "httpRequestHandlers/articleNetworkHandler.h"
#include "httpRequestHandlers/notFoundHandler.h"
#include "httpRequestHandlers/eventNetworkHandler.h"

#include "../core/wikiDataCache.h"

namespace WikiMainPath {

	RoutingHandlerFactory::RoutingHandlerFactory(const WikiDataCache& wiki_data_cache) 
		:_data(wiki_data_cache),
		_article_titles_inverted_index(buildInvertedIndex(wiki_data_cache.article_titles())),
		_category_titles_inverted_index(buildInvertedIndex(wiki_data_cache.category_titles()))
	{}

	Poco::Net::HTTPRequestHandler* RoutingHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest &request)
	{
		Poco::URI uri(request.getURI());

		std::cout << "Received request --- URI: " << uri.getPath() << std::endl;

		if(uri.getPath() == "/search-category")
			return new SearchHandler("matching-categories", _data.category_titles(), _category_titles_inverted_index);

		if(uri.getPath() == "/search-article")
			return new SearchHandler("matching-articles", _data.article_titles(), _article_titles_inverted_index);

		if(uri.getPath() == "/article-list-in-category")
			return new ArticleListHandler(_data.article_titles(), _data.category_has_article());

		// if(uri.getPath() == "/article-network-in-category")
			// return new EventNetworkHandler(_data.article_titles(), _data.dates(), _data.category_has_article(), _data.event_network());
			
		if(uri.getPath() == "/event-network-in-category")
			return new EventNetworkHandler(_data);

		// if(uri.getPath() == "/mpa-in-category")
			// return new MpaHandler(_server_data._articles, _server_data._category_has_article, _server_data._article_network);

		return new NotFoundHandler();
	} 

}
