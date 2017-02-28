#pragma once

#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>
#include <vector>
#include <map>

#include "../../core/date.h"

namespace WikiMainPath {

	enum InfoboxDateExtractionErrorType {
		DATE_TEMPLATE_BUT_NO_DATE_KEY,
		KEY_BUT_NO_DATE_TEMPLATE,
		KEY_AND_DATE_TEMPLATE_TYPES_NOT_MATCHING
	};

	using InfoboxDateExtractionError = std::tuple<InfoboxDateExtractionErrorType, std::string, std::string>;

	std::vector<std::pair<std::string, std::string>> extractAllKeyValuesFromInfobox(const std::string& article_syntax);
	std::vector<Date> extractAllDatesFromInfobox(const std::string& article_syntax, std::vector<InfoboxDateExtractionError>& errors);
	bool extractDateFromString(const std::string& str, Date& date);

}
