#include "searchCategoryHandler.h"

#include <Poco/JSON/Object.h>

namespace WikiMainPath {

	SearchCategoryHandler::SearchCategoryHandler(const InvertedIndex& categories_inverted_index)
		:_categories_inverted_index(categories_inverted_index)
	{}

	void SearchCategoryHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
	{
		using namespace Poco::JSON;

		response.setChunkedTransferEncoding(true);
		response.setContentType("text/json");

		// response.setStatus("200");

		Object root;
		Array matchingCategoriesArray;

		Object el;
		el.set("title", "CategoryTitle");
		el.set("id", "1");

		matchingCategoriesArray.set(0,el);
		root.set("matching-categories", matchingCategoriesArray);

		root.stringify(response.send(), 4); 
	}

}
