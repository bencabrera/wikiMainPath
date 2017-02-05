#pragma once

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include "fullTextSearch.h"

namespace WikiMainPath {

	class SearchCategoryHandler: public Poco::Net::HTTPRequestHandler
	{
		public:
			SearchCategoryHandler(const InvertedIndex& categories_inverted_index);

			void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);

		private:
			const InvertedIndex& _categories_inverted_index;
	};

}
