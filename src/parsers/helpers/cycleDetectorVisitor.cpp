#include "cycleDetectorVisitor.h"

#include <iostream>


CycleDetectorDfsVisitor::CycleDetectorDfsVisitor(std::vector<std::vector<CategoryHirachyGraph::vertex_descriptor>>& cyc)
:_cycles(cyc)
{}

void CycleDetectorDfsVisitor::discover_vertex(CategoryHirachyGraph::vertex_descriptor v, const CategoryHirachyGraph& g)
{
	current_vertices.push(v);
}

void CycleDetectorDfsVisitor::finish_vertex(CategoryHirachyGraph::vertex_descriptor v, const CategoryHirachyGraph& g)
{
	if(current_vertices.top() != v)
		throw std::logic_error("Top vertex is not equal to finish vertex");

	current_vertices.pop();
}

void CycleDetectorDfsVisitor::back_edge(CategoryHirachyGraph::edge_descriptor e, const CategoryHirachyGraph& g)
{
	auto target = boost::target(e, g);

	std::stack<CategoryHirachyGraph::vertex_descriptor> copy = current_vertices;	

	std::list<CategoryHirachyGraph::vertex_descriptor> cycle;

	while(copy.top() != target)
	{
		cycle.push_front(copy.top());
		copy.pop();
	}
	cycle.push_back(target);

	_cycles.push_back(std::vector<CategoryHirachyGraph::vertex_descriptor>(cycle.begin(), cycle.end()));
}
