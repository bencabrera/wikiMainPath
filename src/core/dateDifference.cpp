#include "dateDifference.h"
#include "date.h"

boost::gregorian::date from_tm_to_boost(const std::tm& d)
{
	return boost::gregorian::date(d.tm_year + 1900, d.tm_mon + 1, d.tm_mday);
}

// std::size_t difference_in_days(const std::tm& d1, const std::tm& d2)
// {
	// using namespace boost::gregorian;
	// auto d1_boost = from_tm_to_boost(d1);
	// auto d2_boost = from_tm_to_boost(d2);

	// return (d1_boost - d2_boost).days();
// }

std::size_t difference_in_days(const std::tm& d1, const std::tm& d2)
{
	std::size_t years_diff, months_diff, days_diff;

	if(d1 < d2)
	{
		years_diff = d2.tm_year - d1.tm_year;
		months_diff = d2.tm_mon - d1.tm_mon;
		days_diff = d2.tm_mday - d1.tm_mday;
	}
	else
	{
		years_diff = d1.tm_year - d2.tm_year;
		months_diff = d1.tm_mon - d2.tm_mon;
		days_diff = d1.tm_mday - d2.tm_mday;
	}

	return years_diff * 365 + months_diff * 30 + days_diff;
}
