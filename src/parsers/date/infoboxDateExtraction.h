#pragma once

#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>
#include <vector>
#include <map>

#include "../../core/date.h"

namespace WikiMainPath {

	// enum InfoboxDateExtractionErrorType {
		// DATE_TEMPLATE_BUT_NO_DATE_KEY,
		// KEY_BUT_NO_DATE_TEMPLATE,
		// KEY_AND_DATE_TEMPLATE_TYPES_NOT_MATCHING
	// };

	// const std::vector<std::string> InfoboxDateExtractionErrorLabel = {
		// "DATE_TEMPLATE_BUT_NO_DATE_KEY",
		// "KEY_BUT_NO_DATE_TEMPLATE",
		// "KEY_AND_DATE_TEMPLATE_TYPES_NOT_MATCHING"
	// };

	enum InfoboxDateExtractionErrorType {
		KEY_BUT_NO_DATE_EXTRACTED,
		DATE_EXTRACTED_BUT_NO_KEY
	};

	const std::vector<std::string> InfoboxDateExtractionErrorLabel = {
		"KEY_BUT_NO_DATE_EXTRACTED",
		"DATE_EXTRACTED_BUT_NO_KEY"
	};

	// [key] [prefix]
	const std::map<std::string, std::string> InfoboxExpectedDateKeys = {
		{ "date", "" },
		{ "birth_date", "Birth" },
		{ "death_date", "Death" }
	};

	using InfoboxDateExtractionError = std::tuple<InfoboxDateExtractionErrorType, std::string, std::string>;

	std::vector<std::pair<std::string, std::string>> extract_all_key_values_from_infobox(const std::string& article_syntax);
	std::vector<Date> extract_all_dates_from_infobox(const std::string& article_syntax, std::vector<InfoboxDateExtractionError>& errors);

}
