#include "categorySearchHandler.h"

#include <boost/algorithm/string/trim.hpp>
#include <Poco/JSON/Object.h>

#include <Poco/Net/HTMLForm.h>
#include <Poco/URI.h>

namespace WikiMainPath {

	CategorySearchHandler::CategorySearchHandler(const WikiDataCache& wiki_data_cache, const InvertedIndex& inverted_index)
		:_wiki_data_cache(wiki_data_cache),
		_inverted_index(inverted_index)
	{}

	void CategorySearchHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
	{
		using namespace Poco::JSON;

		response.setChunkedTransferEncoding(true);
		response.setContentType("text/json");
		response.set("Access-Control-Allow-Origin", "*");

		// get parameters
		Poco::URI uri(request.getURI());
		auto parameters = uri.getQueryParameters();

		if(parameters.size() == 0 || parameters[0].first != "query-str")
		{
			response.setStatus("400"); // = Bad Request

			Object root;
			root.set("error", "GET-Parameter query-str not specified or too many parameters.");

			root.stringify(response.send(), 4); 

			return;
		}

		std::string query_str = parameters[0].second;
		boost::trim(query_str);

		std::size_t n_limit = 0;
		if(parameters.size() == 2)
		{
			std::string limit_str = parameters[1].second;
			boost::trim(limit_str);

			n_limit = std::stoul(limit_str);
		}

		DocumentSet results;
		if(query_str != "")
			 results = query(_inverted_index, query_str);

		std::vector<std::size_t> results_ordered(results.begin(), results.end());
		std::sort(results_ordered.begin(), results_ordered.end(), [this](const std::size_t i1, const std::size_t i2) {
			return _wiki_data_cache.category_titles()[i1].size() < _wiki_data_cache.category_titles()[i2].size();
		});

		Object root;
		Array matchingCategoriesArray;

		std::size_t count = 0;
		for (auto res : results_ordered) {
			Object el;
			el.set("title", _wiki_data_cache.category_titles()[res]);
			el.set("id", res);
			el.set("n_articles", _wiki_data_cache.category_has_article()[res].size());

			matchingCategoriesArray.add(el);
			count++;

			if(count > n_limit && n_limit != 0)
				break;
		}

		root.set("matching-categories", matchingCategoriesArray);

		root.stringify(response.send(), 4); 
	}

}
