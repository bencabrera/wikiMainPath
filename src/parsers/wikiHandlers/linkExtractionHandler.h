#pragma once

#include "../xml/abstractArticleHandler.h"
#include "../articleNetwork/linkListExtractor.h"
#include <vector>

#include "../articleNetwork/articleGraph.h"
#include "../articleNetwork/date.h"

class LinkExtractionHandler : public AbstractArticleHandler{
	public:
		typedef DirectedArticleGraph::vertex_descriptor VertexDescriptor;	
		typedef DirectedArticleGraph::edge_descriptor EdgeDescriptor;	

		LinkExtractionHandler(DirectedArticleGraph& g, const std::vector<std::string>& arts, const std::vector<Date>& dates, const std::map<std::string, std::vector<std::string>>& cats, const std::map<std::string, std::string>& redirs);

		void HandleArticle(const ArticleData&);

		bool GetOrAddVertex(const std::string& title, VertexDescriptor& v) const;

		DirectedArticleGraph& graph();

		std::vector<std::pair<std::string,ArticleNetwork::LinkTargetSet>> articles;
	private:
		DirectedArticleGraph& _graph;

		std::vector<std::string> _articles;
		std::vector<Date> _dates;
   		const std::map<std::string, std::vector<std::string>>& _categoriesToArticles;
   		const std::map<std::string, std::string>& _redirects;
};
