#pragma once

#include <vector>
#include <set>
#include <map>

#include <boost/container/flat_set.hpp>

#include "../xml/abstractArticleHandler.h"
#include "../articleNetwork/dateExtractor.h"
#include "../articleNetwork/date.h"
#include "vectorMutex.hpp"

class CategoryHasArticleHandler : public AbstractArticleHandler{
	public:
		CategoryHasArticleHandler(
			const std::vector<std::string>& arts, 
			const std::vector<std::string>& cats, 
			const std::map<std::string, std::string>& rdirs, 
			std::vector<boost::container::flat_set<std::size_t>>& catHasArt, 
			VectorMutex<1000>& vecMut
		);

		void HandleArticle(const ArticleData&);

		const std::vector<std::string>& articles;
		const std::vector<std::string>& categories;
		const std::map<std::string,std::string>& redirects;


		std::vector<boost::container::flat_set<std::size_t>>& categoryHasArticle;


	private:
		inline static bool getPosition(const std::vector<std::string>& vec, std::string str, std::size_t& pos)
		{
			auto it = std::lower_bound(vec.begin(), vec.end(), str);	
			if(it == vec.end())
				return false;
			else
			{
				pos = it - vec.begin();
				return true;
			}
		}

		VectorMutex<1000>& _vecMutex;
};
