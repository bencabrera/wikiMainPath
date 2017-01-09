#include "dateExtraction.h"
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/regex.hpp>

#include <regex>
#include <string>
#include <iomanip>

#include "dateStringGrammar.hpp"

namespace WikiMainPath {

	std::vector<std::string> orderForDateSources = { "date", "birth&death", "birthdate", "born", "death" };

	bool extractDateFromArticle(const std::string& articleSyntax, Date& dateObj) 
	{
		return extractDateFromInfobox(articleSyntax, dateObj);
	}

	bool extractDateFromInfobox(const std::string& articleSyntax, Date& dateObj)
	{
		std::size_t foundPos = articleSyntax.find("{{");
		while(foundPos != std::string::npos)
		{
			std::size_t secondFoundPos = articleSyntax.find("}}", foundPos);
			if(secondFoundPos == std::string::npos)
				break;

			std::string val = articleSyntax.substr(foundPos + 2, secondFoundPos - foundPos - 2);
			boost::trim(val);

			if(val.length() > 7 && boost::to_lower_copy(val.substr(0,7)) == "infobox")
			{
				std::regex infoboxDateStrRegex("[Dd]ate\\s*=\\s*([^\\n]*)");
				std::smatch dateStrMatch;
				if(!std::regex_search(val, dateStrMatch, infoboxDateStrRegex)) 
				{
					foundPos = articleSyntax.find("{{", secondFoundPos);
					continue;
				}

				std::string dateStr = dateStrMatch[1];
				dateStr = boost::regex_replace(dateStr, boost::regex("&\\w+;"), " ");

				auto it = dateStr.begin();	
				boost::spirit::qi::phrase_parse(it, dateStr.end(), DateStringGrammar<std::string::iterator, boost::spirit::qi::blank_type>(), boost::spirit::qi::blank, dateObj);

				if(it == dateStr.end())
					return true;
				// return extractDateFromDateStr(dateStr, dateObj);
			}

			foundPos = articleSyntax.find("{{", secondFoundPos);
		}

		return false;
	}

}

// bool extractDateFromDateStr(std::string dateStr, Date& dateObj) const
// {
// dateObj.IsRange = false;

// const std::vector<std::string> parserFormats = {
// "%d %B %Y",
// "%d %b %Y",
// "%b %d, %Y",
// "%B %d, %Y",
// "%Y"
// };

// // for (auto format : parserFormats) {
// // std::istringstream ss(dateStr);
// // ss >> std::get_time(&dateObj.Begin, format.c_str());	
// // if(!ss.fail())
// // return true;
// // }

// return false;
// }
