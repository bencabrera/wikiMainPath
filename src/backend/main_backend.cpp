#include "routingHandlerFactory.h"

#include <Poco/Util/ServerApplication.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPServer.h>

#include "../core/wikiDataCache.h"
#include <boost/filesystem.hpp>

int main(int, char* argv[])
{
	using namespace WikiMainPath;

	auto data_path = boost::filesystem::path(argv[1]);

	if(!boost::filesystem::is_directory(data_path))
	{
		std::cerr << "Specified folder is non-existent." << std::endl;
		return 1;
	}

	Poco::UInt16 port = 9999;

	Poco::Net::ServerSocket socket(port);

	Poco::Net::HTTPServerParams *pParams = new Poco::Net::HTTPServerParams();
	pParams->setMaxQueued(100);
	pParams->setMaxThreads(16);

	std::cout << "Scanning path " << data_path << " for data files." << std::endl;
	WikiDataCache data(data_path.string());
	data.article_titles();
	data.category_titles();
	data.category_has_article();
	data.article_dates();
	data.event_network();
	data.event_indices();
	data.article_network();
	auto factory = new RoutingHandlerFactory(data);

	Poco::Net::HTTPServer server(factory, socket, pParams);

	server.start();

	std::cout << "Started server on port " << port << std::endl;

	// waitForTerminationRequest();
	while(true){}

	server.stop();

	return 0;
}
