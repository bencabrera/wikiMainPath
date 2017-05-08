#include <functional>
#include <vector>
#include <string>
#include "../core/date.h"

std::function<bool(const std::string&, const std::vector<Date>&)> articles_without_certain_dates(std::vector<std::string> forbidden_date_strs);

std::function<bool(const std::string&, const Date&)> events_in_date_range(Date date_range);
