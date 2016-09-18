#include "wikiDumpHandler.h"
#include <iostream>
#include <boost/algorithm/string/trim.hpp>

WikiDumpHandler::WikiDumpHandler(AbstractArticleHandler& articleHandler, bool extractText)
:_articleHandler(articleHandler),
_insidePage(false),
_extractText(extractText),
articleCount(0)
{}

void WikiDumpHandler::startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const xercesc::Attributes& attrs)
{
	char* tmp = xercesc::XMLString::transcode(localname);
	std::string elementName = tmp;
	xercesc::XMLString::release(&tmp);

	if(elementName == "page")
		_insidePage = true;
	else
		if(_insidePage)
		{
			if(elementName == "redirect"){
				_currentArticleData.IsRedirect = true;
				const char* bla = "title"; 
				XMLCh* titleTmp= xercesc::XMLString::transcode(bla);
				auto tmpAttr = attrs.getValue(titleTmp);
				char* tmpAttr2 = xercesc::XMLString::transcode(tmpAttr);
				_currentArticleData.RedirectTarget = tmpAttr2;
				boost::trim(_currentArticleData.RedirectTarget);
				xercesc::XMLString::release(&tmpAttr2);
				xercesc::XMLString::release(&titleTmp);
			}

			_elementStack.push_back(elementName);
		}
}

void WikiDumpHandler::endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname)
{
	char* tmp = xercesc::XMLString::transcode(localname);
	std::string elementName = tmp;
	xercesc::XMLString::release(&tmp);

	if(elementName == "page")
	{
		if(TitleFilter){
			if(TitleFilter(_currentArticleData.MetaData.at("title")))
				_articleHandler.HandleArticle(_currentArticleData);
		}
		else
			_articleHandler.HandleArticle(_currentArticleData);

		_currentArticleData.MetaData.clear();
		_currentArticleData.IsRedirect = false;
		_currentArticleData.RedirectTarget = "";
		_currentArticleData.Content.clear();
		_insidePage = false;
		_elementStack.clear();
		articleCount++;
		if(articleCount % 1000 == 0)
			std::cerr << "Articles parsed so far: " << articleCount << std::endl;
	}
	else
		if(_insidePage)
			_elementStack.pop_back();
}

void WikiDumpHandler::characters(const XMLCh* const chars, const XMLSize_t length)
{
	if(_elementStack.size() > 0 && (_elementStack.back() != "text" || _extractText))
	{
		char* tmp = xercesc::XMLString::transcode(chars);
		std::string str = tmp;
		xercesc::XMLString::release(&tmp);

		if(_elementStack.back() == "text")
			_currentArticleData.Content += str;
		else 
		{
			boost::trim(str);
			if(_insidePage && !str.empty())
			{
				std::string key = _elementStack[0];
				for(std::size_t i = 1; i < _elementStack.size(); i++)
					key += "." + _elementStack[i];

				while(_currentArticleData.MetaData.find(key) != _currentArticleData.MetaData.end())
					key += ".";

				_currentArticleData.MetaData.insert({ key, str });
			}
		}
	}
}

void WikiDumpHandler::fatalError(const xercesc::SAXParseException& exception)
{
	char* message = xercesc::XMLString::transcode(exception.getMessage());
	std::cout << "Fatal Error: " << message << " at line: " << exception.getLineNumber() << std::endl;
	xercesc::XMLString::release(&message);
}
