#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>

#include "date.h"

class DateExtractor {
public:
	bool operator()(const std::string& articleSyntax, Date& dateObj) const;

private:
	bool extractDateFromInfobox(const std::string& text, Date& dateObj) const;
	bool extractDateFromDateStr(std::string dateStr, Date& dateObj) const;
};
