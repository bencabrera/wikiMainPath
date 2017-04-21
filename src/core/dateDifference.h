#pragma once

#include <ctime>
#include <boost/date_time/gregorian/gregorian_types.hpp>

boost::gregorian::date from_tm_to_boost(const std::tm& d);

std::size_t difference_in_days(const std::tm& d1, const std::tm& d2);
