#pragma once

#include <vector>
#include <set>
#include <map>

#include <boost/container/flat_set.hpp>

#include "../../../libs/wiki_xml_dump_xerces/src/handlers/abstractWikiPageHandler.hpp"
#include "vectorMutex.hpp"

class CategoryHasArticleHandler : public WikiXmlDumpXerces::AbstractWikiPageHandler {
	public:
		CategoryHasArticleHandler(
			const std::vector<std::string>& arts, 
			const std::vector<std::string>& cats, 
			std::vector<boost::container::flat_set<std::size_t>>& catHasArt, 
			VectorMutex<1000>& vecMut
		);

		void HandleArticle(const WikiXmlDumpXerces::WikiPageData&);

		const std::vector<std::string>& articles;
		const std::vector<std::string>& categories;

		std::vector<boost::container::flat_set<std::size_t>>& categoryHasArticle;


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

		VectorMutex<1000>& _vecMutex;
};
