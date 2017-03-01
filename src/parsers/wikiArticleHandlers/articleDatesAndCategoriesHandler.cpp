#include "articleDatesAndCategoriesHandler.h"

#include <boost/algorithm/string/trim.hpp>

#include "../date/articleDateExtraction.h"

ArticleDatesAndCategoriesHandler::ArticleDatesAndCategoriesHandler()
:ExtractOnlyArticlesWithDates(true)
{}

void ArticleDatesAndCategoriesHandler::HandleArticle(const WikiXmlDumpXerces::WikiPageData& data)
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
			categories.push_back(title.substr(9));
		}
		else
		{
			Date dateObj;
			if(WikiMainPath::extractDateFromArticle(data.Content, dateObj) || !ExtractOnlyArticlesWithDates)
			{
				articles.insert({ title, dateObj });
			}
		}
	}
}
