#pragma once

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include <boost/container/flat_set.hpp>
#include "../../core/graph.h"
#include "../../core/wikiDataCache.h"
#include "../serverDataCache.h"

namespace WikiMainPath {

	class EventNetworkHandler: public Poco::Net::HTTPRequestHandler
	{
		public:
			EventNetworkHandler(const WikiDataCache& wiki_data_cache);

			void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);

		private:
			const WikiDataCache& _wiki_data_cache;
	};

}
