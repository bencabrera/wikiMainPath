#include "infoboxDateExtraction.h"

#include <boost/algorithm/string/trim.hpp>

#include "dateStringGrammar.hpp"
#include "infoboxKeyValueGrammar.hpp"
#include "../../core/adaptDate.h"

namespace WikiMainPath {

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

	std::vector<std::pair<std::string, std::string>> extractAllKeyValuesFromInfobox(const std::string& article_syntax)
	{
		std::vector<std::pair<std::string, std::string>> key_values;
		auto it = article_syntax.cbegin();

		try {
			InfoboxKeyValueGrammar<std::string::const_iterator, boost::spirit::qi::blank_type> grammar;
			boost::spirit::qi::phrase_parse(it, article_syntax.cend(), grammar, boost::spirit::qi::blank, key_values);
		}
		catch(boost::spirit::qi::expectation_failure<std::string::const_iterator> e)
		{}

		for(auto& pair : key_values)
		{
			boost::trim(pair.first);
			boost::trim(pair.second);
		}

		return key_values;
	}

	// could add an error category if the descriptions don't match
	std::vector<Date> extractAllDatesFromInfobox(const std::string& article_syntax, std::vector<std::pair<std::string, std::string>>& date_but_no_key, std::vector<std::pair<std::string, std::string>>& key_but_no_date)
	{
		std::vector<Date> rtn;

		WikiMainPath::DateStringGrammar<std::string::const_iterator, boost::spirit::qi::blank_type> date_grammar;

		auto key_values = extractAllKeyValuesFromInfobox(article_syntax);
		for (const auto& pair : key_values) {
			Date d;
			auto it = pair.second.cbegin();
			auto date_key_it = INFOBOX_KEY_TO_DATE_TYPE.find(pair.first);

			try{
				boost::spirit::qi::phrase_parse(it, pair.second.cend(), date_grammar, boost::spirit::qi::blank, d);
			}
			catch(boost::spirit::qi::expectation_failure<std::string::const_iterator> e)
			{}

			if(it != pair.second.cend())
			{
				if(date_key_it != INFOBOX_KEY_TO_DATE_TYPE.end())
					key_but_no_date.push_back(pair);
				continue;
			} else {
				if(date_key_it != INFOBOX_KEY_TO_DATE_TYPE.end())
				{
					// if(date_key_it->second == d.Description)
					d.Description = date_key_it->second;
					rtn.push_back(d);
				} else {
					date_but_no_key.push_back(pair);
				}
			}
		}

		return rtn;
	}

}
