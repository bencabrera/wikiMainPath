#include "randomGraphDrawing.h"
#include <random>

std::vector<double> averaged_precessor_graph_drawing(const EventNetwork& event_network)
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


	for(std::size_t i = 0; i < boost::num_vertices(event_network); i++)
		if(boost::in_degree(i, event_network) == 0)
			continue;
		else {
			// compute average of incoming neighbours
			double avg = 0.0;
			for (auto e : boost::make_iterator_range(boost::in_edges(i,event_network)))
				avg += positions[boost::source(e,event_network)];
			avg = avg / boost::in_degree(i,event_network);

			positions[i] = avg;
		}

	return positions;
}
