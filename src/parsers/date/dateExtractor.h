#pragma once

#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>

#include "date.h"


// #include <boost/config/warning_disable.hpp>
// #include <boost/spirit/include/qi.hpp>
// #include <boost/spirit/include/phoenix_core.hpp>
// #include <boost/spirit/include/phoenix_operator.hpp>
// #include <boost/spirit/include/phoenix_fusion.hpp>
// #include <boost/spirit/include/phoenix_stl.hpp>
// #include <boost/spirit/include/phoenix_object.hpp>
// #include <boost/fusion/include/adapt_struct.hpp>
// #include <boost/variant/recursive_variant.hpp>
// #include <boost/spirit/include/qi_symbols.hpp>
// #include <boost/fusion/include/adapt_struct.hpp>
// #include <boost/fusion/include/io.hpp>

// #include <boost/fusion/adapted/std_pair.hpp>
// #include <boost/fusion/adapted/std_tuple.hpp>

// BOOST_FUSION_ADAPT_STRUCT(
	// std::tm,
	// (int, tm_year)
	// (int, tm_mon)
	// (int, tm_mday)
	// (int, tm_hour)
	// (int, tm_min)
// )

class DateExtractor {
public:
	bool operator()(const std::string& articleSyntax, Date& dateObj) const;

private:
	bool extractDateFromInfobox(const std::string& text, Date& dateObj) const;
	bool extractDateFromDateStr(std::string dateStr, Date& dateObj) const;
};
