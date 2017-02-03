#pragma once

#include <vector>
#include <set>
#include <map>

#include <boost/container/flat_set.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "../../../libs/wiki_xml_dump_xerces/src/handlers/abstractWikiPageHandler.hpp"
#include "vectorMutex.hpp"

class CategoryRecursiveHandler : public WikiXmlDumpXerces::AbstractWikiPageHandler {
	public:
		typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, boost::no_property, boost::no_property, boost::vecS> Graph;

		CategoryRecursiveHandler(
			const std::vector<std::string>& cats, 
			Graph& graph,
			VectorMutex<1000>& vecMut
		);

		void HandleArticle(const WikiXmlDumpXerces::WikiPageData&);

	private:
		inline static bool getPosition(const std::vector<std::string>& vec, std::string str, std::size_t& pos)
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

		const std::vector<std::string>& _categories;
		Graph& _graph;
		VectorMutex<1000>& _vecMutex;
};
