#pragma once

#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>

#include "date.h"

namespace WikiMainPath {

	bool extractDateFromArticle(const std::string& articleSyntax, Date& dateObj);

	bool extractDateFromInfobox(const std::string& articleSyntax, Date& dateObj);

}
