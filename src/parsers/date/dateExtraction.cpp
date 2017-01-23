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

	const std::vector<std::string> DATE_TYPE_ORDER = {
		"BIRTH_DEATH",
		"BIRTH",
		"DEATH",
		"GENERAL_DATE"
	};

	const std::map<std::string, std::string> INFOBOX_KEY_TO_DATE_TYPE = {
		{ "date", "GENERAL_DATE" },
		{ "born", "BIRTH" },
		{ "birth", "BIRTH" },
		{ "birthdate", "BIRTH" },
		{ "birthday", "BIRTH" },
		{ "death", "DEATH" },
		{ "died", "DEATH" },
		{ "deathdate", "DEATH" },
		{ "deathday", "DEATH" }
	};

	// step 1: extracting all possible sources of dates
	std::map<std::string, Date> extractAllDatesFromArticle(const std::string& article_syntax) 
	{
		return extractAllDatesFromInfobox(article_syntax);
	}

	std::vector<std::pair<std::string, std::string>> extractAllKeyValuesFromInfobox(const std::string& article_syntax)
	{
		std::vector<std::pair<std::string, std::string>> key_values;
		auto it = article_syntax.cbegin();
		
		WikiMainPath::InfoboxKeyValueGrammar<std::string::const_iterator, boost::spirit::qi::blank_type> grammar;
		boost::spirit::qi::phrase_parse(it, article_syntax.cend(), grammar, boost::spirit::qi::blank, key_values);

		for(auto& pair : key_values)
		{
			boost::trim(pair.first);
			boost::trim(pair.second);
		}

		return key_values;
	}

	// could add an error category if the descriptions match
	std::vector<Date> extractAllDatesFromInfobox(const std::string& article_syntax, std::vector<std::pair<std::string, std::string>>& date_but_no_key, std::vector<std::pair<std::string, std::string>>& key_but_no_date)
	{
		std::vector<Date> rtn;

		WikiMainPath::DateStringGrammar<std::string::const_iterator, boost::spirit::qi::blank_type> date_grammar;

		auto key_values = extractAllKeyValuesFromInfobox(article_syntax);
		for (const auto& pair : key_values) {
			Date d;
			auto it = pair.second.cbegin();
			auto date_key_it = INFOBOX_KEY_TO_DATE_TYPE.find(pair.first);
			boost::spirit::qi::phrase_parse(it, pair.second.cend(), date_grammar, boost::spirit::qi::blank, d);

			if(it != pair.second.cend())
			{
				if(date_key_it != INFOBOX_KEY_TO_DATE_TYPE.end())
					key_but_no_date.push_back(pair);
				continue;
			} else {
				if(date_key_it != INFOBOX_KEY_TO_DATE_TYPE.end())
				{
					// if(date_key_it->second == d.Description)
					rtn.push_back(d);
				} else {
					date_but_no_key.push_back(pair);
				}
			}
		}
		
		return rtn;
	}

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
	bool extractDateFromArticle(const std::string& article_syntax, Date& date)
	{
		throw std::logic_error("Not implemented");
	}

}
