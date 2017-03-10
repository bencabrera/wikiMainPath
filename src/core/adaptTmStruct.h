#pragma once

#include <ctime>
#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(
	std::tm,
	(int, tm_year)
	(int, tm_mon)
	(int, tm_mday)
	(int, tm_yday) // 0 = full date, 1 = year & month, 2 = year
)
