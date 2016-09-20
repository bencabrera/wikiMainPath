#include "linkExtractionHandler.h"

LinkExtractionHandler::LinkExtractionHandler(DirectedArticleGraph& g, const std::vector<std::string>& arts, const std::vector<std::tm>& dates, const std::map<std::string, std::vector<std::string>>& cats, const std::map<std::string, std::string>& redirs)
:_graph(g),
_articles(arts),
_dates(dates),
_categoriesToArticles(cats),
_redirects(redirs)
{
	if(boost::num_vertices(g) < _articles.size())
		throw std::logic_error("Graph needs to have same number of vertices as articles vector as entries");
}

bool LinkExtractionHandler::GetOrAddVertex(const std::string& title, VertexDescriptor& v) const
{
	auto it = std::lower_bound(_articles.begin(), _articles.end(), title);
	if(*it == title)
	{
		v = it-_articles.begin();
		return true;
	}	
	else
		return false;
}


DirectedArticleGraph& LinkExtractionHandler::graph()
{
	return _graph;
}

void LinkExtractionHandler::HandleArticle(const ArticleData& data)
{
	std::string title = data.MetaData.at("title");
	boost::trim(title);

	VertexDescriptor source, target;
	if(!GetOrAddVertex(title, source))
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
				|| linkTitle.substr(0,5) == "Talk:" 
				|| linkTitle.substr(0,9) == "Category:" 
				|| !GetOrAddVertex(linkTitle, target)
		  )
		{
			foundPos = data.Content.find("[[", secondFoundPos);
			continue;
		}

		if(!boost::edge(source,target,_graph).second)
			boost::add_edge(source, target, _graph);

		foundPos = data.Content.find("[[", secondFoundPos);
	}
}
