#pragma once

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/subgraph.hpp>
#include <boost/graph/copy.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <vector>
#include <string>

#include "../parsers/date/date.h"

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, boost::no_property, boost::property<boost::edge_index_t,std::size_t>, boost::vecS> Graph;
typedef boost::subgraph<Graph> Subgraph;

Subgraph readDataFromFile (
		const boost::filesystem::path& inputFolder, 
		std::vector<std::string>& articles, 
		std::vector<Date>& dates, 
		std::vector<std::string>& categories, 
		std::vector<std::vector<std::size_t>>& category_has_article
);
