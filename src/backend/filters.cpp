#include "filters.h"

#include <boost/algorithm/string/case_conv.hpp>

std::function<bool(const std::string&, const std::vector<Date>&)> articles_without_certain_dates(std::vector<std::string> forbidden_date_strs)
{
	for (auto& forbidden : forbidden_date_strs) {
		boost::to_lower(forbidden);
	}
	return [forbidden_date_strs](const std::string&, const std::vector<Date>& dates) {
		for (auto& d : dates) {
			std::string description = d.Description;
			boost::to_lower(description);

			for (auto forbidden : forbidden_date_strs) {
				if(description.find(forbidden) != std::string::npos)
					return false;
			}
		}
		return true;	
	};
}

std::function<bool(const std::string&, const Date&)> events_in_date_range(Date date_range)
{
	if(!date_range.IsRange)
		throw std::logic_error("'date_range' is not a range");

	return [date_range](const std::string& event_title, const Date& event_date) {
		if(event_date.Begin < date_range.Begin || date_range.End < event_date.Begin)
			return false;
		else
			return true;	
	};
}
