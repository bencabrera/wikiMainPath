#include <boost/test/unit_test.hpp>

#include "../../s3_cycle_detector_visitor.h"

BOOST_AUTO_TEST_SUITE(cycle_detection_tests)

BOOST_AUTO_TEST_CASE(cycle_should_be_found)
{
	CycleDetectorDfsVisitor::Graph graph(10);

	std::vector<std::size_t> edges = { 
		0, 1,
		1, 2,
		2, 3,
		2, 4,
		0, 5,
		5, 6,
		6, 7,
		7, 5,
		5, 8,
		8, 9
	};

	for(std::size_t i = 0; i < edges.size(); i += 2)
		boost::add_edge(edges[i], edges[i+1], graph);

	std::vector<std::vector<CycleDetectorDfsVisitor::Graph::vertex_descriptor>> cycles;
	CycleDetectorDfsVisitor vis(cycles);

	boost::depth_first_search(graph, boost::visitor(vis));
	
	BOOST_CHECK_EQUAL(cycles.size(), 1);

	std::set<CycleDetectorDfsVisitor::Graph::vertex_descriptor> tmp(cycles[0].begin(), cycles[0].end());
	BOOST_CHECK(tmp.find(5) != tmp.end());
	BOOST_CHECK(tmp.find(6) != tmp.end());
	BOOST_CHECK(tmp.find(7) != tmp.end());
}

BOOST_AUTO_TEST_SUITE_END()
