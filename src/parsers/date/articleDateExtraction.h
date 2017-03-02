#pragma once

#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>
#include <vector>
#include <map>

#include "../../core/date.h"
#include "infoboxDateExtraction.h"

namespace WikiMainPath {

	std::vector<Date> extract_all_dates_from_article(const std::string& article_syntax, std::vector<InfoboxDateExtractionError>& errors);





	// extern const std::vector<std::string> DATE_TYPE_ORDER;
	// extern const std::map<std::string, std::string> INFOBOX_KEY_TO_DATE_TYPE;
	// extern const std::vector<std::tuple<std::string, std::string, std::string>> DATE_COMBINATIONS;
	//
	// const std::vector<std::string> DATE_TYPE_ORDER = {
		// "BIRTH_DEATH",
		// "BIRTH",
		// "DEATH",
		// "GENERAL_DATE"
	// };

	// const std::vector<std::tuple<std::string, std::string, std::string>> DATE_COMBINATIONS = {
		// // [older_date_type] [newer_date_type] [combination_type]
		// { "BIRTH", "DEATH", "BIRTH_DEATH" }
	// };

	// step 1: extracting all possible sources of dates

	// // step 2: remove duplicates
	// void removeDuplicateDates(std::vector<Date>& dates);

	// // step 3: combining single dates to ranges
	// void combineSingleDateToRanges(std::vector<Date>& dates);

	// // step 4: chose one date to characterize article
	// bool chooseSingleDate(const std::vector<Date>& dates, Date& date);

	// // step 5: combine all steps
	// bool extractDateFromArticle(const std::string& article_syntax, Date& date);

}
