#pragma once

// boost
#include <boost/container/flat_set.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

namespace std {
	template<typename T>
	T begin(const std::pair<T,T>& eItPair)
	{
		return eItPair.first;
	}
	
	template<typename T>
	T end(const std::pair<T,T>& eItPair)
	{
		return eItPair.second;
	}
}

void recursiveFillCategories(boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, boost::no_property, boost::no_property, boost::vecS>& graph, std::vector<boost::container::flat_set<std::size_t>>& category_has_article);
