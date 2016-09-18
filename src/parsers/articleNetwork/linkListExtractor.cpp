#include "linkListExtractor.h"
#include <boost/algorithm/string/trim.hpp>
#include <iostream>

namespace ArticleNetwork {

	LinkTargetSet LinkListExtractor::operator()(const std::string& content)
	{
		LinkTargetSet rtn;

		std::size_t foundPos = content.find("[[");
		while(foundPos != std::string::npos)
		{
			std::size_t secondFoundPos = content.find("]]", foundPos);
			if(secondFoundPos == std::string::npos)
				break;

			std::string val = content.substr(foundPos + 2, secondFoundPos - foundPos - 2);

			std::size_t pipePos = val.find("|");
			if(pipePos != std::string::npos)
				val = val.substr(0, pipePos);

			boost::trim(val);

			rtn.insert(val);

			foundPos = content.find("[[", secondFoundPos);
		}
		
		return rtn;
	}

}
