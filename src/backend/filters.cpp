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
