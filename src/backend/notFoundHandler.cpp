#include "notFoundHandler.h"

#include <Poco/DateTime.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/DateTimeFormat.h>

void NotFoundHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
	response.setChunkedTransferEncoding(true);
	response.setContentType("text/html");

	response.setStatus("404");

	std::ostream& responseStream = response.send();
	responseStream << "<html>";
	responseStream << "<head><title>WikiMainPath - 404 Not Found</title></head>";
	responseStream << "<body><h1>The request route could not be found on the server.</h1><body>";
	responseStream << "</html>";
}
