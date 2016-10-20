#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>

#include <stack>

class CycleDetectorDfsVisitor : public boost::default_dfs_visitor
{
	public:
		typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, boost::no_property, boost::no_property, boost::vecS> Graph;

		CycleDetectorDfsVisitor(std::vector<std::vector<Graph::vertex_descriptor>>& cyc);

		std::stack<Graph::vertex_descriptor> current_vertices;

		void discover_vertex(Graph::vertex_descriptor v, const Graph& g);

		void finish_vertex(Graph::vertex_descriptor v, const Graph& g);

		void back_edge(Graph::edge_descriptor e, const Graph& g);

		std::vector<std::vector<Graph::vertex_descriptor>>& _cycles;
};


