#pragma once

#include <Poco/Net/HTTPRequestHandlerFactory.h>

#include "../core/wikiDataCache.h"
#include "fullTextSearch.h"

namespace WikiMainPath {

	class RoutingHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory 
	{ 
		public: 
			RoutingHandlerFactory(const WikiDataCache& wiki_data_cache);

			Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest &request);

		private:
			const WikiDataCache& _data;
			const InvertedIndex _article_titles_inverted_index;
			const InvertedIndex _category_titles_inverted_index;
	};

}
