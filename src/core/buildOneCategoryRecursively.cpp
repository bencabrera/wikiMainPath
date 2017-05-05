#include "buildOneCategoryRecursively.h"
#include <iostream>
#include <algorithm>

#include <boost/graph/depth_first_search.hpp>

std::set<std::size_t> build_one_category_recursively(const std::size_t i_category, const CategoryHirachyGraph& graph, const std::vector<std::vector<std::size_t>>& category_has_article)
{
	class CategoryTree : public boost::default_dfs_visitor
	{
		public:
			void finish_vertex(CategoryHirachyGraph::vertex_descriptor v, const CategoryHirachyGraph& g)
			{
				categories_in_tree.push_back(v);
			}

			std::vector<CategoryHirachyGraph::vertex_descriptor> categories_in_tree;
	};

	CategoryTree tree;
	boost::depth_first_search(graph, boost::visitor(tree));

	std::set<std::size_t> articles_in_category;
	for (auto i_subcategory : tree.categories_in_tree) {
		articles_in_category.insert(category_has_article[i_subcategory].begin(), category_has_article[i_subcategory].end());	
	}

	return articles_in_category; 
}
