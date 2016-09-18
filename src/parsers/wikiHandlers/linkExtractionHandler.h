#pragma once

#include "../xml/abstractArticleHandler.h"
#include "../articleNetwork/linkListExtractor.h"
#include <vector>

#include "../articleNetwork/articleGraph.h"


class LinkExtractionHandler : public AbstractArticleHandler{
	public:
		typedef UndirectedArticleGraph::vertex_descriptor VertexDescriptor;	
		typedef UndirectedArticleGraph::edge_descriptor EdgeDescriptor;	

		LinkExtractionHandler(UndirectedArticleGraph& g);

		void HandleArticle(const ArticleData&);

		VertexDescriptor GetOrAddVertex(const std::string& title);

		UndirectedArticleGraph& graph();

		std::vector<std::pair<std::string,ArticleNetwork::LinkTargetSet>> articles;
	private:
		ArticleNetwork::LinkListExtractor _linkListExtractor;

		UndirectedArticleGraph& _graph;
		std::map<std::string,VertexDescriptor> _vMap;
};
