#include "searchHandler.h"

#include <boost/algorithm/string/trim.hpp>
#include <Poco/JSON/Object.h>

#include <Poco/Net/HTMLForm.h>
#include <Poco/URI.h>

namespace WikiMainPath {

	SearchHandler::SearchHandler(const std::string json_attribute_name, const std::vector<std::string>& vec, const InvertedIndex& inverted_index)
		:_json_attribute_name(json_attribute_name),
		_vector(vec),
		_inverted_index(inverted_index)
	{}

	void SearchHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
	{
		using namespace Poco::JSON;

		response.setChunkedTransferEncoding(true);
		response.setContentType("text/json");

		// get parameters
		Poco::URI uri(request.getURI());
		auto parameters = uri.getQueryParameters();

		if(parameters.size() != 1 || parameters[0].first != "query-str")
		{
			response.setStatus("400"); // = Bad Request

			Object root;
			root.set("error", "GET-Parameter query-str not specified or too many parameters.");

			root.stringify(response.send(), 4); 

			return;
		}

		std::string query_str = parameters[0].second;
		boost::trim(query_str);

		DocumentSet results;
		if(query_str != "")
			 results = query(_inverted_index, query_str);

		Object root;
		Array matchingCategoriesArray;

		for (auto res : results) {
			Object el;
			el.set("title", _vector[res]);
			el.set("id", res);

			matchingCategoriesArray.add(el);
		}

		root.set(_json_attribute_name, matchingCategoriesArray);

		root.stringify(response.send(), 4); 
	}

}
