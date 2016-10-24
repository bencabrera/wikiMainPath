#include "linkExtractionHandler.h"

LinkExtractionHandler::LinkExtractionHandler(
	const std::vector<std::string>& arts, 
	const std::vector<Date>& dates, 
	const std::map<std::string, std::string>& redirs,
	std::vector<boost::container::flat_set<std::size_t>>& adjList,
	VectorMutex<1000>& vecMut
)
:_articles(arts),
_dates(dates),
_redirects(redirs),
_adjList(adjList),
_vecMutex(vecMut)
{
	if(_adjList.size() != _articles.size())
		throw std::logic_error("Dimensions are not correct.");

	// default behaviour use dates to order
	OrderCallback = [this](std::size_t source, std::size_t target)
	{
		return (this->_dates[source] < this->_dates[target]);
	};
}

bool LinkExtractionHandler::GetPosition(const std::string& title, std::size_t& v) const
{
	auto it = std::lower_bound(_articles.begin(), _articles.end(), title);
	if(it != _articles.end() && *it == title)
	{
		v = it-_articles.begin();
		return true;
	}	
	else
		return false;
}

void LinkExtractionHandler::HandleArticle(const ArticleData& data)
{
	std::string title = data.MetaData.at("title");
	boost::trim(title);

	std::size_t source, target;
	if(!GetPosition(title, source))
		return;

	std::size_t foundPos = data.Content.find("[[");
	while(foundPos != std::string::npos)
	{
		std::size_t secondFoundPos = data.Content.find("]]", foundPos);
		if(secondFoundPos == std::string::npos)
			break;

		std::string linkTitle = data.Content.substr(foundPos + 2, secondFoundPos - foundPos - 2);

		std::size_t pipePos = linkTitle.find("|");
		if(pipePos != std::string::npos)
			linkTitle = linkTitle.substr(0, pipePos);

		boost::trim(linkTitle);

		if(
				linkTitle.substr(0,5) == "User:" 
				|| linkTitle.substr(0,10) == "Wikipedia:" 
				|| linkTitle.substr(0,5) == "File:" 
				|| linkTitle.substr(0,9) == "Category:"
				|| linkTitle.substr(0,9) == "Template:"
				|| linkTitle.substr(0,14) == "Template talk:"
				|| linkTitle.substr(0,14) == "Category talk:" 
				|| linkTitle.substr(0,10) == "User talk:"
				|| linkTitle.substr(0,10) == "File talk:"
				|| linkTitle.substr(0,15) == "Wikipedia talk:"
				|| !GetPosition(linkTitle, target)
		  )
		{
			foundPos = data.Content.find("[[", secondFoundPos);
			continue;
		}

		if(OrderCallback(source, target))
		{
			_vecMutex.lock(source);
			_adjList[source].insert(target);
			_vecMutex.unlock(source);
		}
		else
		{
			_vecMutex.lock(target);
			_adjList[target].insert(source);
			_vecMutex.unlock(target);
		}

		foundPos = data.Content.find("[[", secondFoundPos);
	}
}
