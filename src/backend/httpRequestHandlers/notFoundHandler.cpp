#include "notFoundHandler.h"

#include <Poco/JSON/Object.h>

namespace WikiMainPath {

	void NotFoundHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
	{
		using namespace Poco::JSON;

		response.setChunkedTransferEncoding(true);
		response.setContentType("text/json");

		response.setStatus("404");

		Object root;
		root.set("error", "Route not found on server.");

		root.stringify(response.send(), 4); 
	}

}
