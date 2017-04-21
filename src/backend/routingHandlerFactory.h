#pragma once

#include <Poco/Net/HTTPRequestHandlerFactory.h>

#include "../core/wikiDataCache.h"
#include "fullTextSearch.h"
#include "serverDataCache.h"

namespace WikiMainPath {

	class RoutingHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory 
	{ 
		public: 
			RoutingHandlerFactory(const WikiDataCache& wiki_data_cache, ServerDataCache& server_data_cache);

			Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest &request);

		private:
			const WikiDataCache& _wiki_data_cache;
			ServerDataCache& _server_data_cache;
			const InvertedIndex _article_titles_inverted_index;
			const InvertedIndex _category_titles_inverted_index;
	};

}
