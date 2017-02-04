#include "mpaHandlerFactory.h"

#include "mpaRequestHandler.h"

MpaHandlerFactory::MpaHandlerFactory() 
{
}

Poco::Net::HTTPRequestHandler* MpaHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest &request)
{
	return new MpaRequestHandler();
} 
