#include "randomGraphDrawing.h"
#include <random>

namespace WikiMainPath {
	namespace GraphDrawing {
		std::vector<double> random_graph_drawing(const EventNetwork& event_network)
		{
			std::mt19937 rand_gen(3);

			std::uniform_real_distribution<double> unif_pos;

			std::vector<double> rtn;
			for(std::size_t i = 0; i < boost::num_vertices(event_network); i++)
				rtn.push_back(unif_pos(rand_gen));

			return rtn;
		}

	}
}
