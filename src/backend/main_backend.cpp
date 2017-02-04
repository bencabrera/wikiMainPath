#include "mpaHandlerFactory.h"

#include <Poco/Util/ServerApplication.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPServer.h>

int main(int argc, char* argv[])
{
	Poco::UInt16 port = 9999;

	Poco::Net::ServerSocket socket(port);

	Poco::Net::HTTPServerParams *pParams = new Poco::Net::HTTPServerParams();
	pParams->setMaxQueued(100);
	pParams->setMaxThreads(16);

	Poco::Net::HTTPServer server(new MpaHandlerFactory(), socket, pParams);

	server.start();

	// waitForTerminationRequest();
	while(true){}

	server.stop();

	return 0;
}
