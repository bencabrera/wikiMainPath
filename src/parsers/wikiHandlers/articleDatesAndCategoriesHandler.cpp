#include "articleDatesAndCategoriesHandler.h"

#include <boost/algorithm/string/trim.hpp>

void ArticleDatesAndCategoriesHandler::HandleArticle(const ArticleData& data)
{
	std::string title = data.MetaData.at("title");
	boost::trim(title);

	// if it is redirect, safe it to the redirects map for later
	if(data.IsRedirect)
	{
		redirects.insert({ title, data.RedirectTarget });
	}
	else
	{
		if(title.size() > 9 && title.substr(0,9) == "Category:")
		{
			categories.push_back(title.substr(10));
		}
		else
		{
			Date dateObj;
			if(extractDate(data.Content, dateObj))
			{
				articles.insert({ title, dateObj });
			}
		}
	}
}
