#pragma once

#include <stack>
#include <vector>
#include <functional>

#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax2/Attributes.hpp>

#include "abstractArticleHandler.h"

class CountPagesHandler : public xercesc::DefaultHandler {
	public:
		void startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const xercesc::Attributes& attrs);

		CountPagesHandler();
		std::size_t count() const;
	private:
		std::size_t _count;
};

