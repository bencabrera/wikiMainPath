#pragma once

#include <iostream>
#include <algorithm>

#include "graph.h"
#include <boost/container/flat_set.hpp>

std::set<std::size_t> build_one_category_recursively(const std::size_t i_category, const CategoryHirachyGraph& graph, const std::vector<std::vector<std::size_t>>& category_has_article);
