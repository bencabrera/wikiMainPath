#pragma once

#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>
#include <vector>
#include <map>

#include "../../core/date.h"

namespace WikiMainPath {

	extern const std::vector<std::string> DATE_TYPE_ORDER;
	extern const std::map<std::string, std::string> INFOBOX_KEY_TO_DATE_TYPE;
	extern const std::vector<std::tuple<std::string, std::string, std::string>> DATE_COMBINATIONS;

	// step 1: extracting all possible sources of dates
	std::vector<Date> extractAllDatesFromArticle(const std::string& article_syntax, std::vector<std::pair<std::string, std::string>>& date_but_no_key, std::vector<std::pair<std::string, std::string>>& key_but_no_date);
	std::vector<std::pair<std::string, std::string>> extractAllKeyValuesFromInfobox(const std::string& article_syntax);
	std::vector<Date> extractAllDatesFromInfobox(const std::string& article_syntax, std::vector<std::pair<std::string, std::string>>& date_but_no_key, std::vector<std::pair<std::string, std::string>>& key_but_no_date);

	// step 2: remove duplicates
	void removeDuplicateDates(std::vector<Date>& dates);

	// step 3: combining single dates to ranges
	void combineSingleDateToRanges(std::vector<Date>& dates);

	// step 4: chose one date to characterize article
	bool chooseSingleDate(const std::vector<Date>& dates, Date& date);

	// step 5: combine all steps
	bool extractDateFromArticle(const std::string& article_syntax, Date& date, std::vector<std::pair<std::string, std::string>>& date_but_no_key, std::vector<std::pair<std::string, std::string>>& key_but_no_date);

}
