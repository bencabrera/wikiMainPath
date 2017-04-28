#include <boost/test/unit_test.hpp>

#include <fstream>
#include <streambuf>
#include <string>
#include <iostream>

#include "../graphDrawing/forceDirectedGraphDrawing.h"

BOOST_AUTO_TEST_SUITE(force_directed_tests)

BOOST_AUTO_TEST_CASE(isolated_vertex_should_be_place_at_middle)
{
	ArticleGraph g;
	boost::add_vertex(g);

	std::vector<double> x_positions = { 0.5 };

	auto y = force_directed_graph_drawing(g, x_positions);	

	BOOST_TEST(y[0] == 0.5, boost::test_tools::tolerance(0.01));
}

BOOST_AUTO_TEST_CASE(test2)
{
	ArticleGraph g;
	auto v1 = boost::add_vertex(g);
	auto v2 = boost::add_vertex(g);
	boost::add_edge(v1, v2, g);

	std::vector<double> x_positions = { 0.5, 0.5 };

	auto y = force_directed_graph_drawing(g, x_positions);	

	std::cout << "Positions: " << std::endl;
	std::cout << y[0] << ", " << y[1] << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()
