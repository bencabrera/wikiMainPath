#pragma once

#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>

#include "date.h"

namespace WikiMainPath {

	// step 1: extracting all possible sources of dates
	std::map<std::string, Date> extractAllDatesFromArticle(const std::string& articleSyntax);
	std::map<std::string, Date> extractAllDatesFromInfobox(const std::string& articleSyntax);
	std::vector<std::pair<std::string, std::string>> extractAllKeyValuesFromInfobox(const std::string& articleSyntax);

	// step 2: combining single dates to ranges
	void combineSingleDateToRanges(std::map<std::string, Date>& dates);

	// step 3: chose one date to characterize article
	Date combineSingleDateToRanges(const std::map<std::string, Date>& dates);

	// step 4: combine all steps
	bool extractDateFromArticle(const std::string& articleSyntax, Date& date);

}
