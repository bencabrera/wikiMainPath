#include "mpaHandlerFactory.h"

#include <Poco/Util/ServerApplication.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPServer.h>

int main(int argc, char* argv[])
{
	using namespace WikiMainPath;

	std::string data_path = argv[1];

	Poco::UInt16 port = 9999;

	Poco::Net::ServerSocket socket(port);

	Poco::Net::HTTPServerParams *pParams = new Poco::Net::HTTPServerParams();
	pParams->setMaxQueued(100);
	pParams->setMaxThreads(16);

	std::cout << "Scanning path " << data_path << " for data files." << std::endl;
	auto factory = new MpaHandlerFactory(data_path);

	Poco::Net::HTTPServer server(factory, socket, pParams);

	server.start();

	std::cout << "Started server on port " << port << std::endl;

	// waitForTerminationRequest();
	while(true){}

	server.stop();

	return 0;
}
