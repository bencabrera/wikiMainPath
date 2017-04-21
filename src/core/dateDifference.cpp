#include "dateDifference.h"

boost::gregorian::date from_tm_to_boost(const std::tm& d)
{
	return boost::gregorian::date(d.tm_year + 1900, d.tm_mon + 1, d.tm_mday);
}

std::size_t difference_in_days(const std::tm& d1, const std::tm& d2)
{
	using namespace boost::gregorian;
	auto d1_boost = from_tm_to_boost(d1);
	auto d2_boost = from_tm_to_boost(d2);

	return (d1_boost - d2_boost).days();
}
