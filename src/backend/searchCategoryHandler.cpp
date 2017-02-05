#include "searchCategoryHandler.h"

#include <Poco/JSON/Object.h>

#include <Poco/Net/HTMLForm.h>
#include <Poco/URI.h>

namespace WikiMainPath {

	SearchCategoryHandler::SearchCategoryHandler(const std::vector<std::string>& categories, const InvertedIndex& categories_inverted_index)
		:_categories(categories),
		_categories_inverted_index(categories_inverted_index)
	{}

	void SearchCategoryHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
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
		std::cout << "QUERY STR: " << query_str << std::endl;
		auto results = query(_categories_inverted_index, query_str);

		Object root;
		Array matchingCategoriesArray;

		for (auto res : results) {
			Object el;
			el.set("title", _categories[res]);
			el.set("id", res);

			matchingCategoriesArray.add(el);
		}

		root.set("matching-categories", matchingCategoriesArray);

		root.stringify(response.send(), 4); 
	}

}
