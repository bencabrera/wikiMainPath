#include <boost/test/unit_test.hpp>

#include <fstream>
#include <streambuf>
#include <string>
#include <iostream>
#include <random>

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

BOOST_AUTO_TEST_CASE(test3)
{
	ArticleGraph g;
	auto v1 = boost::add_vertex(g);
	auto v2 = boost::add_vertex(g);
	auto v3 = boost::add_vertex(g);
	auto v4 = boost::add_vertex(g);
	boost::add_edge(v1, v2, g);
	boost::add_edge(v2, v3, g);
	boost::add_edge(v1, v4, g);
	boost::add_edge(v2, v4, g);
	boost::add_edge(v4, v3, g);

	std::vector<double> x_positions = { 0.2, 0.4, 0.6, 0.8 };

	ForceDirectedFunctor f(g, x_positions);
	Eigen::NumericalDiff<ForceDirectedFunctor> f_numerical_diff(f);

	ForceDirectedFunctor::InputType y(f.inputs());

	std::mt19937 rand_gen(42);
	std::uniform_real_distribution<double> unif_0_1;
	for(int i = 0; i < y.size(); i++)
		y(i) = unif_0_1(rand_gen);
	
	ForceDirectedFunctor::JacobianType mat1(f.values(), f.inputs()), mat2(f.values(), f.inputs());
	f.df(y,mat1);
	f_numerical_diff.df(y,mat2);

	ForceDirectedFunctor::JacobianType m = mat1 - mat2;
	for(int i = 0; i < m.rows(); i++)
		for(int j = 0; j < m.cols(); j++)
			if(m(i,j) < 1e-7)
				m(i,j) = 0.0;

	BOOST_TEST(m.norm() == 0.0, boost::test_tools::tolerance(1e-10));
}

BOOST_AUTO_TEST_SUITE_END()
