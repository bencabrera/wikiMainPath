#include "articleTitlesHandler.h"

#include <boost/algorithm/string/trim.hpp>

void ArticleTitlesHandler::HandleArticle(const ArticleData& data)
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
		// if it is no redirect, check if a date can be extracted
		std::tm dateObj;
		if(extractDate(data.Content, dateObj))
		{
			// add article
			articles.insert({ title, dateObj });

			// run through categories and add article to this categories
			std::size_t foundPos = data.Content.find("[[Category:");
			while(foundPos != std::string::npos)
			{
				std::size_t secondFoundPos = data.Content.find("]]", foundPos);
				if(secondFoundPos == std::string::npos)
					break;

				std::string category_title = data.Content.substr(foundPos + 11, secondFoundPos - foundPos - 11);
				boost::trim(category_title);

				auto map_it = categoriesToArticles.find(category_title);
				if(map_it == categoriesToArticles.end())
					categoriesToArticles.insert({ category_title, std::vector<std::string>{ title }});	
				else
					map_it->second.push_back(title);

				foundPos = data.Content.find("[[Category:", secondFoundPos);
			}
		}
	}
}
