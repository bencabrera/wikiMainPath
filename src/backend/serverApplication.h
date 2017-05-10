#pragma once

#include <Poco/Util/ServerApplication.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPServer.h>

#include "routingHandlerFactory.h"
#include "../core/wikiDataCache.h"
#include "fullTextSearch.h"
#include "serverDataCache.h"
#include "filters.h"

class Server : public Poco::Util::ServerApplication 
{
	private:
		const WikiMainPath::WikiDataCache& _wiki_data_cache;
		ServerDataCache _server_data_cache;


	public:

		Server(const WikiMainPath::WikiDataCache& wiki_data_cache)
			:_wiki_data_cache(wiki_data_cache),
			_server_data_cache(wiki_data_cache)
		{
			// _server_data_cache.article_filters.push_back([](const std::string&, const std::vector<Date>& dates) {
				// return true;	
			// });

			_server_data_cache.article_filters.push_back(articles_without_certain_dates({ "Birth", "Death", "Released", "Recorded", "First aired", "Last aired", "Term started", "Term ended" }));
			_server_data_cache.event_filters.push_back(events_in_date_range(create_date_range(1780,1,1,1820,1,1)));
		}

		void initialize(Application& self)
		{
			loadConfiguration();
			ServerApplication::initialize(self);
		}

		void uninitialize()
		{
			ServerApplication::uninitialize();
		}

		int main(const std::vector<std::string>& args)
		{
			Poco::UInt16 port = 9999;

			Poco::Net::ServerSocket socket(port);

			Poco::Net::HTTPServerParams *pParams = new Poco::Net::HTTPServerParams();
			pParams->setMaxQueued(100);
			pParams->setMaxThreads(16);
			pParams->setKeepAlive(true);
			pParams->setTimeout(100000);

			auto handler_factor = new WikiMainPath::RoutingHandlerFactory(_wiki_data_cache, _server_data_cache);

			Poco::Net::HTTPServer server(handler_factor, socket, pParams);

			server.start();
			waitForTerminationRequest();
			server.stop();

			return Application::EXIT_OK;
		}
};
