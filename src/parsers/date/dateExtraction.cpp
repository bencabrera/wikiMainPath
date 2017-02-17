#include "dateExtraction.h"
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/regex.hpp>

#include <regex>
#include <string>
#include <iomanip>

#include "dateStringGrammar.hpp"
#include "infoboxKeyValueGrammar.hpp"
#include "../../core/adaptDate.h"
#include "infoboxDateExtraction.h"

namespace WikiMainPath {

	const std::vector<std::string> DATE_TYPE_ORDER = {
		"BIRTH_DEATH",
		"BIRTH",
		"DEATH",
		"GENERAL_DATE"
	};

	const std::vector<std::tuple<std::string, std::string, std::string>> DATE_COMBINATIONS = {
		// [older_date_type] [newer_date_type] [combination_type]
		{ "BIRTH", "DEATH", "BIRTH_DEATH" }
	};

	// step 1: extracting all possible sources of dates
	std::vector<Date> extractAllDatesFromArticle(const std::string& article_syntax, std::vector<std::pair<std::string, std::string>>& date_but_no_key, std::vector<std::pair<std::string, std::string>>& key_but_no_date) 
	{
		return extractAllDatesFromInfobox(article_syntax, date_but_no_key, key_but_no_date);
	}


	// step 2: remove duplicates
	void removeDuplicateDates(std::vector<Date>& dates) 
	{
		std::set<Date, std::function<bool(const Date&, const Date&)>> unique_dates(dates.begin(), dates.end(), [](const Date& d1, const Date& d2) {
			return d1.Description < d2.Description;
		});

		dates = std::vector<Date>(unique_dates.begin(), unique_dates.end());
	}

	// step 3: combining single dates to ranges
	void combineSingleDateToRanges(std::vector<Date>& dates)
	{
		std::map<std::string, Date> descr_to_date;
		for (auto d : dates) 
			descr_to_date.insert({ d.Description, d });

		for (auto combined_type : DATE_COMBINATIONS) {
			auto it1 = descr_to_date.find(std::get<0>(combined_type));
			auto it2 = descr_to_date.find(std::get<1>(combined_type));
			if(it1 != descr_to_date.end() && it2 != descr_to_date.end())
			{
				Date new_combined_date;
				new_combined_date.Description = std::get<2>(combined_type);
				new_combined_date.IsRange = true;
				new_combined_date.Begin = it1->second.Begin;
				new_combined_date.End = it2->second.Begin;

				dates.push_back(new_combined_date);
			}
		}	
	}

	// step 4: chose one date to characterize article
	bool chooseSingleDate(const std::vector<Date>& dates, Date& date)
	{
		if(dates.empty())
			return false;

		static std::map<std::string, std::size_t> date_type_to_pos;
		if(date_type_to_pos.empty()) {
			for (std::size_t i = 0; i < DATE_TYPE_ORDER.size(); i++) 
				date_type_to_pos.insert({ DATE_TYPE_ORDER[i], i });
		}

		auto it = std::max_element(dates.begin(), dates.end(), [](const Date& d1, const Date& d2) {
			return date_type_to_pos[d1.Description] < date_type_to_pos[d2.Description];					
		});

		date = *it;

		return true;
	}


	// step 5: combine all steps
	bool extractDateFromArticle(const std::string& article_syntax, Date& date, std::vector<std::pair<std::string, std::string>>& date_but_no_key, std::vector<std::pair<std::string, std::string>>& key_but_no_date)
	{
		auto dates = extractAllDatesFromArticle(article_syntax, date_but_no_key, key_but_no_date);
		removeDuplicateDates(dates);
		combineSingleDateToRanges(dates);
		return chooseSingleDate(dates, date);
	}

}
