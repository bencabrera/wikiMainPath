#pragma once

#include <stack>
#include <vector>
#include <functional>

#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax2/Attributes.hpp>

#include "abstractArticleHandler.h"

class WikiDumpHandler : public xercesc::DefaultHandler {
	public:
		WikiDumpHandler(AbstractArticleHandler& articleHandler, bool extractText = false);

		void startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const xercesc::Attributes& attrs);
		void endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname);
		void characters(const XMLCh* const chars, const XMLSize_t length);
		void fatalError(const xercesc::SAXParseException&);

		std::function<bool(const std::string&)> TitleFilter;
	private:
		AbstractArticleHandler& _articleHandler;
		ArticleData _currentArticleData;
		std::vector<std::string> _elementStack;
		bool _insidePage;
		bool _extractText;
		std::size_t articleCount;
};

