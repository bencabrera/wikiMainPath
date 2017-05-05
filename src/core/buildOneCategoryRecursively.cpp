#include "buildOneCategoryRecursively.h"
#include <iostream>
#include <algorithm>

#include <boost/graph/depth_first_search.hpp>
#include <boost/property_map/vector_property_map.hpp>

std::set<std::size_t> build_one_category_recursively(const std::size_t i_category, const CategoryHirachyGraph& graph, const std::vector<std::vector<std::size_t>>& category_has_article)
{
	class CategoryTree : public boost::default_dfs_visitor
	{
		public:
			CategoryTree(const std::vector<std::vector<std::size_t>>& category_has_article, std::set<std::size_t>& s)
				:_category_has_article(category_has_article),
				articles_in_category(s)
			{}

			void discover_vertex(CategoryHirachyGraph::vertex_descriptor v, const CategoryHirachyGraph& g)
			{
				articles_in_category.insert(_category_has_article[v].begin(), _category_has_article[v].end());	
			}

			const std::vector<std::vector<std::size_t>>& _category_has_article;
			std::set<std::size_t>& articles_in_category;
	};

	std::set<std::size_t> articles_in_category;
	CategoryTree tree(category_has_article, articles_in_category);
	boost::vector_property_map<boost::default_color_type> color_map;
	boost::depth_first_visit(graph, i_category, tree, color_map);

	return articles_in_category; 
}
