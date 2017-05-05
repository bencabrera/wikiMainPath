#include "buildRecursiveCategories.h"
#include <iostream>
#include <algorithm>

#include "cycleDetectorVisitor.h"


std::set<std::size_t> build_one_category_recursively(const std::size_t i_category, const CategoryHirachyGraph& graph, const std::vector<boost::container::flat_set<std::size_t>>& category_has_article)
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

void build_category_hirachy_graph_recursively(CategoryHirachyGraph& graph, std::vector<boost::container::flat_set<std::size_t>>& category_has_article)
{
	// check if there are no cycles left
	std::vector<std::vector<CategoryHirachyGraph::vertex_descriptor>> cycles;
	CycleDetectorDfsVisitor vis(cycles);
	boost::depth_first_search(graph, boost::visitor(vis));
	if(cycles.size() > 0)
		throw std::logic_error("There are cycles left in CategoryHirachyGraph");

	// do the following until all vertices are isolated
	bool allIsolated = false;
	while(!allIsolated)
	{
		allIsolated = true;
		for(std::size_t v = 0; v < boost::num_vertices(graph); v++)
		{
			if(boost::out_degree(v,graph) == 0 && boost::in_degree(v,graph) == 0)
				continue;

			// if algo reaches this point then not all vertices are isolated
			allIsolated = false;

			// has no outgoing edges => starting point to insert categories into each other
			if(boost::out_degree(v,graph) == 0)
			{
				for(auto e : boost::make_iterator_range(boost::in_edges(v,graph)))
				{
					std::size_t parentIdx = boost::source(e,graph);		
					std::size_t childIdx = boost::target(e,graph);		

					category_has_article[parentIdx].insert(category_has_article[childIdx].begin(), category_has_article[childIdx].end());
				}	

				boost::clear_vertex(v, graph);
			}
		}
	}
}
