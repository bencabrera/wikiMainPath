#pragma once

// boost
#include <boost/container/flat_set.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "../../core/graph.h"

void remove_cycles_in_category_hirachy(CategoryHirachyGraph& graph);
