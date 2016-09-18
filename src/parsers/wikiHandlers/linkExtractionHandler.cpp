#include "linkExtractionHandler.h"

LinkExtractionHandler::LinkExtractionHandler(UndirectedArticleGraph& g)
:_graph(g)
{}

LinkExtractionHandler::VertexDescriptor LinkExtractionHandler::GetOrAddVertex(const std::string& title)
{
	auto it = _vMap.find(title);
	if(it == _vMap.end())
	{
		auto v = boost::add_vertex(_graph);
		boost::put(boost::vertex_name, _graph, v, title);
		_vMap.insert({ title, v });

		return v;
	}	
	else
		return it->second;
}


UndirectedArticleGraph& LinkExtractionHandler::graph()
{
	return _graph;
}

void LinkExtractionHandler::HandleArticle(const ArticleData& data)
{
	std::string title = data.MetaData.at("title");
	boost::trim(title);

	if(title.substr(0,8) == "Category" || title.substr(0,4) == "User" || title.substr(0,9) == "Wikipedia" || title.substr(0,4) == "Talk" || title.substr(0,4) == "File")
		return;

	auto source = GetOrAddVertex(title);

	auto linkStrList = _linkListExtractor(data.Content);
	for (auto linkStr : linkStrList) {
		if(linkStr.substr(0,8) == "Category" || linkStr.substr(0,4) == "User" || linkStr.substr(0,9) == "Wikipedia" || linkStr.substr(0,4) == "Talk" || linkStr.substr(0,4) == "File")
			continue;

		auto target = GetOrAddVertex(linkStr);	
		
		if(!boost::edge(source,target,_graph).second)
			boost::add_edge(source, target, _graph);
	}
}
