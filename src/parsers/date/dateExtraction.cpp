#include "dateExtraction.h"
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/regex.hpp>

#include <regex>
#include <string>
#include <iomanip>

#include "dateStringGrammar.hpp"
#include "infoboxKeyValueGrammar.hpp"

namespace WikiMainPath {


	// step 1: extracting all possible sources of dates
	std::map<std::string, Date> extractAllDatesFromArticle(const std::string& articleSyntax) 
	{
		return extractAllDatesFromInfobox(articleSyntax);
	}

	std::vector<std::pair<std::string, std::string>> extractAllKeyValuesFromInfobox(const std::string& articleSyntax)
	{
		std::vector<std::pair<std::string, std::string>> key_values;
		auto it = articleSyntax.cbegin();
		
		WikiMainPath::InfoboxKeyValueGrammar<std::string::const_iterator, boost::spirit::qi::blank_type> grammar;
		boost::spirit::qi::phrase_parse(it, articleSyntax.cend(), grammar, boost::spirit::qi::blank, key_values);

		for(auto& pair : key_values)
		{
			boost::trim(pair.first);
			boost::trim(pair.second);
		}

		return key_values;
	}

	std::map<std::string, Date> extractAllDatesFromInfobox(const std::string& articleSyntax)
	{
		throw std::logic_error("Not implemented");
		// std::vector<std::pair<std::string, Date>> d;
		// auto it = articleSyntax.cbegin();
		
		// WikiMainPath::InfoboxKeyValueGrammar<std::string::const_iterator, boost::spirit::qi::blank_type> grammar;
		// boost::spirit::qi::phrase_parse(it, articleSyntax.cend(), grammar, boost::spirit::qi::blank, d);

		// std::cout << "AFTER PARSED" << std::endl;
		// for (auto line : d) {
			// std::cout << line.first << ": " << line.second << std::endl;
		// }

		// std::cout << "AFTER COUT" << std::endl;
		// std::map<std::string, Date> rtn;
		// for (auto extr_date : d) 
			// rtn.insert(extr_date);
			
		// return rtn;


		// std::size_t foundPos = articleSyntax.find("{{");
		// while(foundPos != std::string::npos)
		// {
			// std::size_t secondFoundPos = articleSyntax.find("}}", foundPos);
			// if(secondFoundPos == std::string::npos)
				// break;

			// std::string val = articleSyntax.substr(foundPos + 2, secondFoundPos - foundPos - 2);
			// boost::trim(val);

			// if(val.length() > 7 && boost::to_lower_copy(val.substr(0,7)) == "infobox")
			// {
				// std::regex infoboxDateStrRegex("[Dd]ate\\s*=\\s*([^\\n]*)");
				// std::smatch dateStrMatch;
				// if(!std::regex_search(val, dateStrMatch, infoboxDateStrRegex)) 
				// {
					// foundPos = articleSyntax.find("{{", secondFoundPos);
					// continue;
				// }

				// std::string dateStr = dateStrMatch[1];
				// dateStr = boost::regex_replace(dateStr, boost::regex("&\\w+;"), " ");

				// auto it = dateStr.begin();	
				// boost::spirit::qi::phrase_parse(it, dateStr.end(), DateStringGrammar<std::string::iterator, boost::spirit::qi::blank_type>(), boost::spirit::qi::blank, dateObj);

				// if(it == dateStr.end())
					// return true;
				// // return extractDateFromDateStr(dateStr, dateObj);
			// }

			// foundPos = articleSyntax.find("{{", secondFoundPos);
		// }

		// return false;
	}


	std::vector<std::string> orderForDateSources = { "date", "birth&death", "birthdate", "born", "death" };

	// step 2: combining single dates to ranges
	void combineSingleDateToRanges(std::map<std::string, Date>& dates)
	{
		throw std::logic_error("Not implemented");
	}

	// step 3: chose one date to characterize article
	Date combineSingleDateToRanges(const std::map<std::string, Date>& dates)
	{
		throw std::logic_error("Not implemented");
	}


	// step 4: combine all steps
	bool extractDateFromArticle(const std::string& articleSyntax, Date& date)
	{
		throw std::logic_error("Not implemented");
	}

}
