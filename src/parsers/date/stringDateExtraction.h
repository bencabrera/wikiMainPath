#pragma once

#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>
#include <vector>
#include <map>

#include "../../core/date.h"

namespace WikiMainPath {

	bool extractDateFromString(const std::string& str, Date& date);

}
