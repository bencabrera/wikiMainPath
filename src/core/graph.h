#pragma once

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/subgraph.hpp>
#include <boost/graph/copy.hpp>

using ArticleGraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, boost::no_property, boost::property<boost::edge_index_t,std::size_t>, boost::vecS>;
using CategoryHirachyGraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, boost::no_property, boost::no_property, boost::vecS> ;
