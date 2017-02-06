#pragma once

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include "../fullTextSearch.h"

namespace WikiMainPath {

	class SearchCategoryHandler: public Poco::Net::HTTPRequestHandler
	{
		public:
			SearchCategoryHandler(const std::vector<std::string>& categories, const InvertedIndex& categories_inverted_index);

			void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);

		private:
			const std::vector<std::string>& _categories;
			const InvertedIndex& _categories_inverted_index;
	};

}
