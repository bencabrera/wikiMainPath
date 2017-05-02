#include "s3_recursiveFillCategories.h"
#include <iostream>

#include "s3_cycle_detector_visitor.h"

void recursiveFillCategories(boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, boost::no_property, boost::no_property, boost::vecS>& graph, std::vector<boost::container::flat_set<std::size_t>>& category_has_article)
{
	std::vector<std::vector<CycleDetectorDfsVisitor::Graph::vertex_descriptor>> cycles;
	do {
		cycles.clear();

		CycleDetectorDfsVisitor vis(cycles);

		boost::depth_first_search(graph, boost::visitor(vis));
		
		if(cycles.size() > 0)
		{
			auto max_i = 0;
			auto max_v = cycles[0][0];
			auto max_value = boost::out_degree(cycles[0][0], graph);
			for(std::size_t i = 0; i < cycles[0].size(); i++)
			{
				auto v = cycles[0][i];
				auto value = boost::out_degree(v, graph);

				if(value > max_value)
				{
					max_value = value;
					max_v = v;
					max_i = i;
				}
			}

			auto target_v = cycles[0][(max_i + 1) % cycles[0].size()]; 

			auto tmp = boost::edge(max_v, target_v, graph);
			if(!tmp.second)
				throw std::logic_error("Edge was not found...");

			boost::remove_edge(max_v, target_v, graph);
		}
	}
	while(cycles.size() > 0);



	bool allIsolated = false;
	while(!allIsolated)
	{
		allIsolated = true;
		for(std::size_t v = 0; v < boost::num_vertices(graph); v++)
		{
			if(boost::out_degree(v,graph) == 0 && boost::in_degree(v,graph) == 0)
				continue;

			// if algo reaches this point then not all vertices are isolated
			allIsolated = false;

			// has no outgoing edges
			if(boost::out_degree(v,graph) == 0)
			{
				for(auto e : boost::in_edges(v,graph))
				{
					std::size_t parentIdx = boost::source(e,graph);		
					std::size_t childIdx = boost::target(e,graph);		

					category_has_article[parentIdx].insert(category_has_article[childIdx].begin(), category_has_article[childIdx].end());
				}	

				boost::clear_vertex(v, graph);
			}
		}
	}
}
