#include "categoryRecursiveHandler.h"

#include <algorithm>

#include <boost/algorithm/string/trim.hpp>

CategoryRecursiveHandler::CategoryRecursiveHandler(
	const std::vector<std::string>& cats, 
	Graph& graph,
	VectorMutex<1000>& vecMut
)
:_categories(cats),
_graph(graph),
_vecMutex(vecMut)
{}


void CategoryRecursiveHandler::HandleArticle(const WikiXmlDumpXerces::WikiPageData& data)
{
	//std::cout << "hallo" << std::endl;
	if(data.IsRedirect)
		return;

	std::string childTitle = data.MetaData.at("title");
	boost::trim(childTitle);
	childTitle = childTitle.substr(9);
	//std::cout << "always child: " << childTitle << std::endl;

	std::size_t childIdx;
	if(!getPosition(_categories, childTitle, childIdx))
	{
		//std::cout << "child: " << childTitle << std::endl;
		return;
	}

	std::size_t foundPos = data.Content.find("[[Category:");
	while(foundPos != std::string::npos)
	{
		std::size_t secondFoundPos = std::min(data.Content.find("]]", foundPos), data.Content.find("|", foundPos));
		if(secondFoundPos == std::string::npos)
			break;

		std::string parent_title = data.Content.substr(foundPos + 11, secondFoundPos - foundPos - 11);
		boost::trim(parent_title);

		//std::cout << "always: " << parent_title << std::endl;

		std::size_t parentIdx;
		if(getPosition(_categories, parent_title, parentIdx))
		{
			std::size_t minIdx = std::min(childIdx % 1000, parentIdx % 1000);
			std::size_t maxIdx = std::max(childIdx % 1000, parentIdx % 1000);

			if(minIdx == maxIdx)
				_vecMutex.lock(minIdx);
			else
			{
				_vecMutex.lock(minIdx);
				_vecMutex.lock(maxIdx);
			}

			boost::add_edge(parentIdx, childIdx, _graph);
			_vecMutex.unlock(maxIdx);
			_vecMutex.unlock(minIdx);

			if(minIdx == maxIdx)
				_vecMutex.unlock(minIdx);
			else
			{
				_vecMutex.unlock(minIdx);
				_vecMutex.unlock(maxIdx);
			}

		}

		foundPos = data.Content.find("[[Category:", secondFoundPos);
	}
}
