#pragma once

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include "../fullTextSearch.h"

namespace WikiMainPath {

	class SearchHandler: public Poco::Net::HTTPRequestHandler
	{
		public:
			SearchHandler(const std::string json_attribute_name, const std::vector<std::string>& vec, const InvertedIndex& inverted_index);

			void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);

		private:
			const std::string _json_attribute_name;
			const std::vector<std::string>& _vector;
			const InvertedIndex& _inverted_index;
	};

}
