#include "countPagesHandler.h"
#include <iostream>


CountPagesHandler::CountPagesHandler()
:_count(0)
{}
		
std::size_t CountPagesHandler::count() const
{
	return _count;
}

void CountPagesHandler::startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const xercesc::Attributes& attrs)
{
	char* tmp = xercesc::XMLString::transcode(localname);
	std::string elementName = tmp;
	xercesc::XMLString::release(&tmp);

	if(elementName == "page")
		_count++;
}

































































