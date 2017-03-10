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

}
