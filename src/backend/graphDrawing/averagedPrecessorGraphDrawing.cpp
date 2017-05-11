#include "randomGraphDrawing.h"
#include <random>
#include <boost/graph/topological_sort.hpp>
#include <boost/range/adaptor/reversed.hpp>


namespace WikiMainPath {
	namespace GraphDrawing {
		std::vector<double> averaged_precessor_graph_drawing(const ArticleGraph& event_network)
		{
			std::size_t n_wo_incoming = 0;
			for(std::size_t i = 0; i < boost::num_vertices(event_network); i++)
				if(boost::in_degree(i, event_network) == 0)
					n_wo_incoming++;

			double cur_pos = 0;
			std::vector<double> positions(boost::num_vertices(event_network), 0.0);
			for(std::size_t i = 0; i < boost::num_vertices(event_network); i++)
				if(boost::in_degree(i, event_network) == 0)
				{
					cur_pos += 1.0 / (n_wo_incoming+1);
					positions[i] = cur_pos;
				}

			// std::vector<ArticleGraph::vertex_descriptor> topological_order;
			// boost::topological_sort(event_network, std::back_inserter(topological_order));

			// for(std::size_t v = 0; v < boost::num_vertices(event_network); v++)
			for(auto v : boost::make_iterator_range(boost::vertices(event_network))) // output from boost::topological_sort is in reverse top. order => reverse again
			// for(auto v : boost::adaptors::reverse(topological_order)) // output from boost::topological_sort is in reverse top. order => reverse again
				if(boost::in_degree(v, event_network) == 0)
					continue;
				else {
					// compute average of incoming neighbours
					double avg = 0.0;
					for (auto e : boost::make_iterator_range(boost::in_edges(v,event_network)))
						avg += positions[boost::source(e,event_network)];
					avg = avg / boost::in_degree(v,event_network);

					positions[v] = avg;
				}

			return positions;
		}

	} 
}
