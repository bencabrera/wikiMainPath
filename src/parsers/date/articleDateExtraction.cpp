#include "articleDateExtraction.h"
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/regex.hpp>

#include <regex>
#include <string>
#include <iomanip>

#include "../../core/adaptDate.h"

namespace WikiMainPath {

	// TODO: add some special cases (e.g. birth date and age, etc.) 
	// where the split should not append (start) or (end) but it should put e.g. birth, death
	void split_range_dates(std::vector<Date>& dates)
	{
		for (auto date : dates) {
			if(date.IsRange) {
				Date d1, d2;
				d1.Init();
				d2.Init();
				d1.Begin = date.Begin;
				d2.Begin = date.End;
				d1.Description = date.Description + " (Start)";
				d2.Description = date.Description + " (End)";
				dates.push_back(d1);
				dates.push_back(d2);
			}	
		}	

		std::remove_if(dates.begin(), dates.end(), [](const Date& d) { return d.IsRange; });
	}

	// warning: only works for non-range dates (which should be all we need because of split_range step
	std::vector<Date> remove_duplicate_dates(const std::vector<Date>& dates) 
	{
		std::set<Date, std::function<bool(const Date&, const Date&)>> unique_dates([](const Date& d1, const Date& d2) {
			return  d1.Begin < d2.Begin;
		});

		for (auto date : dates) {
			unique_dates.insert(date);	
		}

		return std::vector<Date>(unique_dates.begin(), unique_dates.end());
	}

	// interface to outside: extracting all possible dates
	std::vector<Date> extract_all_dates_from_article(const std::string& article_syntax, std::vector<InfoboxDateExtractionError>& errors) 
	{
		auto extracted_dates = extract_all_dates_from_infobox(article_syntax, errors);

		split_range_dates(extracted_dates);
		auto unique_dates = remove_duplicate_dates(extracted_dates);

		return unique_dates;
	}



	// // step 2: remove duplicates
	// void removeDuplicateDates(std::vector<Date>& dates) 
	// {
		// std::set<Date, std::function<bool(const Date&, const Date&)>> unique_dates(dates.begin(), dates.end(), [](const Date& d1, const Date& d2) {
			// return d1.Description < d2.Description;
		// });

		// dates = std::vector<Date>(unique_dates.begin(), unique_dates.end());
	// }

	// // step 3: combining single dates to ranges
	// void combineSingleDateToRanges(std::vector<Date>& dates)
	// {
		// std::map<std::string, Date> descr_to_date;
		// for (auto d : dates) 
			// descr_to_date.insert({ d.Description, d });

		// for (auto combined_type : DATE_COMBINATIONS) {
			// auto it1 = descr_to_date.find(std::get<0>(combined_type));
			// auto it2 = descr_to_date.find(std::get<1>(combined_type));
			// if(it1 != descr_to_date.end() && it2 != descr_to_date.end())
			// {
				// Date new_combined_date;
				// new_combined_date.Description = std::get<2>(combined_type);
				// new_combined_date.IsRange = true;
				// new_combined_date.Begin = it1->second.Begin;
				// new_combined_date.End = it2->second.Begin;

				// dates.push_back(new_combined_date);
			// }
		// }	
	// }

	// // step 4: chose one date to characterize article
	// bool chooseSingleDate(const std::vector<Date>& dates, Date& date)
	// {
		// if(dates.empty())
			// return false;

		// static std::map<std::string, std::size_t> date_type_to_pos;
		// if(date_type_to_pos.empty()) {
			// for (std::size_t i = 0; i < DATE_TYPE_ORDER.size(); i++) 
				// date_type_to_pos.insert({ DATE_TYPE_ORDER[i], i });
		// }

		// auto it = std::max_element(dates.begin(), dates.end(), [](const Date& d1, const Date& d2) {
			// return date_type_to_pos[d1.Description] > date_type_to_pos[d2.Description];
		// });

		// date = *it;

		// return true;
	// }


	// // step 5: combine all steps
	// bool extractDateFromArticle(const std::string& article_syntax, Date& date)
	// {
		// auto dates = extractAllDatesFromArticle(article_syntax);
		// removeDuplicateDates(dates);
		// combineSingleDateToRanges(dates);
		// return chooseSingleDate(dates, date);
	// }

}
