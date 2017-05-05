#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>

#include <stack>

#include "../../core/graph.h"

class CycleDetectorDfsVisitor : public boost::default_dfs_visitor
{
	public:
		CycleDetectorDfsVisitor(std::vector<std::vector<CategoryHirachyGraph::vertex_descriptor>>& cyc);

		std::stack<CategoryHirachyGraph::vertex_descriptor> current_vertices;

		void discover_vertex(CategoryHirachyGraph::vertex_descriptor v, const CategoryHirachyGraph& g);

		void finish_vertex(CategoryHirachyGraph::vertex_descriptor v, const CategoryHirachyGraph& g);

		void back_edge(CategoryHirachyGraph::edge_descriptor e, const CategoryHirachyGraph& g);

		std::vector<std::vector<CategoryHirachyGraph::vertex_descriptor>>& _cycles;
};


