#pragma once

// boost
#include <boost/container/flat_set.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "../../core/graph.h"

std::set<std::size_t> build_one_category_recursively(const std::size_t i_category, const CategoryHirachyGraph& graph, const std::vector<boost::container::flat_set<std::size_t>>& category_has_article);
void build_category_hirachy_graph_recursively(CategoryHirachyGraph& graph, std::vector<boost::container::flat_set<std::size_t>>& category_has_article);
