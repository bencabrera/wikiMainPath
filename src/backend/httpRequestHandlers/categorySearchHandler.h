#pragma once

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include "../fullTextSearch.h"
#include "../../core/wikiDataCache.h"

namespace WikiMainPath {

	class CategorySearchHandler: public Poco::Net::HTTPRequestHandler
	{
		public:
			CategorySearchHandler(const WikiDataCache& wiki_data_cache, const InvertedIndex& inverted_index);

			void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);

		private:
			const WikiDataCache& _wiki_data_cache;
			const InvertedIndex& _inverted_index;
	};

}
