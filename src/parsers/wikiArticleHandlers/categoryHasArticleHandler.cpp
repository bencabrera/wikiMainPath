#include "categoryHasArticleHandler.h"

#include <iostream>
#include <boost/algorithm/string/trim.hpp>

CategoryHasArticleHandler::CategoryHasArticleHandler(
		const std::vector<std::string>& arts, 
		const std::vector<std::string>& cats, 
		std::vector<boost::container::flat_set<std::size_t>>& catHasArt, 
		VectorMutex<1000>& vecMut
)
:articles(arts),
categories(cats),
categoryHasArticle(catHasArt),
_vecMutex(vecMut)
{
	if(catHasArt.size() != categories.size())
		throw std::logic_error("categoryHasArticle has to have the same dimensions as categories.");
}


void CategoryHasArticleHandler::HandleArticle(const WikiXmlDumpXerces::WikiPageData& data)
{
	std::string title = data.MetaData.at("title");
	boost::trim(title);

	std::size_t articleIdx;
	if(!getPosition(articles, title, articleIdx))
		return;

	if(data.IsRedirect)
		return;

	// run through categories and add article to this categories
	std::size_t foundPos = data.Content.find("[[Category:");
	while(foundPos != std::string::npos)
	{
		std::size_t secondFoundPos = std::min(data.Content.find("]]", foundPos), data.Content.find("|", foundPos));
		if(secondFoundPos == std::string::npos)
			break;

		std::string category_title = data.Content.substr(foundPos + 11, secondFoundPos - foundPos - 11);
		boost::trim(category_title);

		std::size_t categoryIdx;
		if(getPosition(categories, category_title, categoryIdx))
		{
			_vecMutex.lock(categoryIdx);
			categoryHasArticle[categoryIdx].insert(articleIdx);			
			_vecMutex.unlock(categoryIdx);
		}

		foundPos = data.Content.find("[[Category:", secondFoundPos);
	}
}
