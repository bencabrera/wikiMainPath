#pragma once

#include <Poco/Net/HTTPRequestHandlerFactory.h>

class MpaHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory 
{ 
public: 
	MpaHandlerFactory();

	Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest &request);
};
