#include "removeCyclesInCategoryHirachy.h"

#include <iostream>
#include <algorithm>

#include "cycleDetectorVisitor.h"

void remove_cycles_in_category_hirachy(CategoryHirachyGraph& graph)
{
	std::vector<std::vector<CategoryHirachyGraph::vertex_descriptor>> cycles;
	do {
		cycles.clear();

		CycleDetectorDfsVisitor vis(cycles);

		boost::depth_first_search(graph, boost::visitor(vis));
		std::cout << "cycles " << cycles.size() << std::endl;
		
		if(cycles.size() > 0)
		{
			// extract all non-overlapping cycles
			std::set<CategoryHirachyGraph::vertex_descriptor> visited_vertices;
			std::vector<std::size_t> non_overlapping_cycles;
			for(std::size_t i = 0; i < cycles.size(); i++)
			{
				std::set<CategoryHirachyGraph::vertex_descriptor> vertices_in_cycle(cycles[i].begin(), cycles[i].end());
				std::vector<CategoryHirachyGraph::vertex_descriptor> overlap;
				std::set_intersection(vertices_in_cycle.begin(), visited_vertices.end(), overlap.begin(), overlap.end(), std::back_inserter(overlap));
				if(overlap.size() == 0)
					non_overlapping_cycles.push_back(i);
			}

			std::cout << "computed non_overlapping_cycles" << std::endl;

			for (auto i : non_overlapping_cycles) {
				auto& cycle = cycles[i];

				// find vertex in cycle with the lowest out_degree, because edges go parent -> child, 
				// thus the vertex with the lowest out_degree hast the least categories under it
				auto min_v_it = std::min_element(cycle.begin(), cycle.end(), [&graph] (const CategoryHirachyGraph::vertex_descriptor& v1, const CategoryHirachyGraph::vertex_descriptor& v2) {
					return boost::out_degree(v1, graph) < boost::out_degree(v2, graph); 
				});
				
				auto next_v_it = min_v_it;
				next_v_it++;
				if(next_v_it == cycle.end())
					next_v_it = cycle.begin();

				if(boost::edge(*min_v_it, *next_v_it, graph).second)
					boost::remove_edge(*min_v_it, *next_v_it, graph);
			}
		}
	}
	while(cycles.size() > 0);
}
