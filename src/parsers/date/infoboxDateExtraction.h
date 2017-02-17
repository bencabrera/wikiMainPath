#pragma once

#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>
#include <vector>
#include <map>

#include "../../core/date.h"

namespace WikiMainPath {

	std::vector<std::pair<std::string, std::string>> extractAllKeyValuesFromInfobox(const std::string& article_syntax);
	std::vector<Date> extractAllDatesFromInfobox(const std::string& article_syntax, std::vector<std::pair<std::string, std::string>>& date_but_no_key, std::vector<std::pair<std::string, std::string>>& key_but_no_date);

}
