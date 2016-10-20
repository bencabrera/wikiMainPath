#include "s3_cycle_detector_visitor.h"

#include <iostream>


CycleDetectorDfsVisitor::CycleDetectorDfsVisitor(std::vector<std::vector<Graph::vertex_descriptor>>& cyc)
:_cycles(cyc)
{}

void CycleDetectorDfsVisitor::discover_vertex(Graph::vertex_descriptor v, const Graph& g)
{
	current_vertices.push(v);
}

void CycleDetectorDfsVisitor::finish_vertex(Graph::vertex_descriptor v, const Graph& g)
{
	if(current_vertices.top() != v)
		throw std::logic_error("Top vertex is not equal to finish vertex");

	current_vertices.pop();
}

void CycleDetectorDfsVisitor::back_edge(Graph::edge_descriptor e, const Graph& g)
{
	auto target = boost::target(e, g);

	std::stack<Graph::vertex_descriptor> copy = current_vertices;	

	std::list<Graph::vertex_descriptor> cycle;

	while(copy.top() != target)
	{
		cycle.push_front(copy.top());
		copy.pop();
	}
	cycle.push_back(target);

	_cycles.push_back(std::vector<Graph::vertex_descriptor>(cycle.begin(), cycle.end()));
}
