#pragma once

#include <vector>
#include <set>
#include <map>

#include <boost/container/flat_set.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "../../../libs/wiki_xml_dump_xerces/src/handlers/abstractWikiPageHandler.hpp"
#include "vectorMutex.hpp"

class AllLinksArticleHander : public WikiXmlDumpXerces::AbstractWikiPageHandler {
	public:

		typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, boost::no_property, boost::no_property, boost::vecS> CategoryHirachyGraph;

		AllLinksArticleHander(
			const std::vector<std::string>& article_titles, 
			const std::vector<std::string>& category_titles, 
			const std::map<std::string, std::string>& redirs,
			std::vector<boost::container::flat_set<std::size_t>>& category_has_article, 
			CategoryHirachyGraph& category_hirachy_graph,
			std::vector<boost::container::flat_set<std::size_t>>& article_adjacency_list,
			VectorMutex<1000>& vecMut
		);

		void HandleArticle(const WikiXmlDumpXerces::WikiPageData&);


	private:
		static std::vector<std::string> parse_all_links(const std::string& content);

		inline static bool get_position(const std::vector<std::string>& vec, std::string str, std::size_t& pos)
		{
			auto it = std::lower_bound(vec.begin(), vec.end(), str);	
			if(it == vec.end() || *it != str)
				return false;
			else
			{
				pos = it - vec.begin();
				return true;
			}
		}

		const std::vector<std::string>& _article_titles;
		const std::vector<std::string>& _category_titles;
		const std::map<std::string, std::string>& _redirects;
		std::vector<boost::container::flat_set<std::size_t>>& _category_has_article;
		CategoryHirachyGraph& _category_hirachy_graph;
		std::vector<boost::container::flat_set<std::size_t>>& _article_adjacency_list;
		VectorMutex<1000>& _vecMutex;
};
