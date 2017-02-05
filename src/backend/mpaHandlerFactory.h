#pragma once

#include <Poco/Net/HTTPRequestHandlerFactory.h>

#include "serverData.h"

namespace WikiMainPath {

	class MpaHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory 
	{ 
		public: 
			MpaHandlerFactory(const ServerData& server_data);

			Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest &request);

		private:
			const ServerData& _server_data;
	};

}
