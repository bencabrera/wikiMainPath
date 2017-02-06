#pragma once

#include <Poco/Net/HTTPRequestHandlerFactory.h>

#include "serverData.h"

namespace WikiMainPath {

	class RoutingHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory 
	{ 
		public: 
			RoutingHandlerFactory(const ServerData& server_data);

			Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest &request);

		private:
			const ServerData& _server_data;
	};

}
