#include "articleListHandler.h"

#include <Poco/JSON/Object.h>

#include <Poco/Net/HTMLForm.h>
#include <Poco/URI.h>

namespace WikiMainPath {

	ArticleListHandler::ArticleListHandler(const std::vector<std::string>& articles, const std::vector<std::vector<std::size_t>>& category_has_article)
		:_articles(articles),
		_category_has_article(category_has_article)
	{}

	void ArticleListHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
	{
		using namespace Poco::JSON;

		response.setChunkedTransferEncoding(true);
		response.setContentType("text/json");

		// get parameters
		Poco::URI uri(request.getURI());
		auto parameters = uri.getQueryParameters();

		if(parameters.size() != 1 || parameters[0].first != "category-id")
		{
			response.setStatus("400"); // = Bad Request

			Object root;
			root.set("error", "GET-Parameter category-id not specified or too many parameters.");

			root.stringify(response.send(), 4); 

			return;
		}

		std::string category_id_str = parameters[0].second;
		std::size_t category_id = std::stoul(category_id_str);

		if(category_id >= _category_has_article.size())
		{
			response.setStatus("400"); // = Bad Request

			Object root;
			root.set("error", "GET-Parameter category-id was larger than the sizes of _category_has_article.");

			root.stringify(response.send(), 4); 

			return;
		}

		Object root;
		Array articles_array;

		for (auto article_id : _category_has_article[category_id]) {

			if(article_id >= _articles.size())
			{
				response.setStatus("400"); // = Bad Request

				Object root;
				root.set("error", "The article_id '" + std::to_string(article_id) + "' from _category_has_article[" + std::to_string(category_id) + "] exceeds the size of _articles (size() = " + std::to_string(_articles.size()) + ")");

				root.stringify(response.send(), 4); 

				return;
			}

			Object el;
			el.set("title", _articles[article_id]);
			el.set("id", article_id);

			articles_array.add(el);
		}

		root.set("articles", articles_array);

		root.stringify(response.send(), 4); 
	}

}
