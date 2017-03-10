#include "infoboxDateExtraction.h"

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include "grammars/infoboxKeyValueGrammar.hpp"
#include "stringDateExtraction.h"

namespace WikiMainPath {

	// const std::map<std::string, std::string> INFOBOX_KEY_TO_DATE_TYPE = {
		// { "date", "GENERAL_DATE" },
		// { "born", "BIRTH" },
		// { "birth", "BIRTH" },
		// { "birthdate", "BIRTH" },
		// { "birth_date", "BIRTH" },
		// { "birthday", "BIRTH" },
		// { "death", "DEATH" },
		// { "died", "DEATH" },
		// { "deathdate", "DEATH" },
		// { "death_date", "DEATH" },
		// { "deathday", "DEATH" }
	// };
	//
	// could add an error category if the descriptions don't match
	// std::vector<Date> extractAllDatesFromInfobox(const std::string& article_syntax, std::vector<InfoboxDateExtractionError>& errors)
	// {
		// std::vector<Date> rtn;

		// auto key_values = extractAllKeyValuesFromInfobox(article_syntax);
		// for (const auto& pair : key_values) {
			// if(pair.second.empty())
				// continue;

			// Date d;
			// auto date_key_it = INFOBOX_KEY_TO_DATE_TYPE.find(pair.first);

			// if(!extractDateFromString(pair.second,d))
			// {
				// // date could NOT be parsed
				
				// // key of infobox key value pair indicates date, but could not be parsed
				// if(date_key_it != INFOBOX_KEY_TO_DATE_TYPE.end())
					// errors.push_back({ KEY_BUT_NO_DATE_TEMPLATE, std::get<0>(pair), std::get<1>(pair) });
			// } 
			// else 
			// {
				// // date could be parsed
				// if(date_key_it != INFOBOX_KEY_TO_DATE_TYPE.end())
				// {
					// if(date_key_it->second != d.Description)
					// {
						// // key indicated date, could be parsed, but the date types did not match
						// errors.push_back({ KEY_AND_DATE_TEMPLATE_TYPES_NOT_MATCHING, std::get<0>(pair), std::get<1>(pair) });
					// }

					// d.Description = date_key_it->second;
					// rtn.push_back(d);
				// } else {
					// // a date could be parsed, but key did not indicate a date
					// errors.push_back({ DATE_TEMPLATE_BUT_NO_DATE_KEY, std::get<0>(pair), std::get<1>(pair) });
				// }
			// }
		// }

		// return rtn;
	// }

	std::vector<std::pair<std::string, std::string>> extract_all_key_values_from_infobox(const std::string& article_syntax)
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
	std::vector<Date> extract_all_dates_from_infobox(const std::string& article_syntax, std::vector<InfoboxDateExtractionError>& errors)
	{
		std::vector<Date> rtn;

		auto key_values = extract_all_key_values_from_infobox(article_syntax);
		for (const auto& pair : key_values) 
		{
			auto key = pair.first;
			boost::to_lower(key);

			if(pair.second.empty())
				continue;

			auto date_key_it = InfoboxExpectedDateKeys.find(key);

			Date d;
			if(!extractDateFromString(pair.second,d))
			{
				// date COULD NOT be parsed
				
				// key of infobox key value pair indicates date, but could not be parsed
				if(date_key_it != InfoboxExpectedDateKeys.end())
					errors.push_back({ KEY_BUT_NO_DATE_EXTRACTED, std::get<0>(pair), std::get<1>(pair) });
			} 
			else 
			{
				// date COULD be parsed
				if(date_key_it == InfoboxExpectedDateKeys.end())
				{
					// a date could be parsed, but key did not indicate a date
					errors.push_back({ DATE_EXTRACTED_BUT_NO_KEY, std::get<0>(pair), std::get<1>(pair) });
					d.Description = pair.first;
				}
				else
					d.Description = date_key_it->second;

				rtn.push_back(d);
			}
		}

		return rtn;
	}

}
