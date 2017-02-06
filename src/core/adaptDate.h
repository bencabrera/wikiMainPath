#pragma once

#include <boost/fusion/include/adapt_struct.hpp>

// #include <boost/config/warning_disable.hpp>
// #include <boost/spirit/include/qi.hpp>
// #include <boost/spirit/include/phoenix_core.hpp>
// #include <boost/spirit/include/phoenix_operator.hpp>
// #include <boost/spirit/include/phoenix_fusion.hpp>
// #include <boost/spirit/include/phoenix_stl.hpp>
// #include <boost/spirit/include/phoenix_object.hpp>
// // #include <boost/fusion/include/io.hpp>
// #include <boost/fusion/include/adapt_struct.hpp>
// #include <boost/fusion/adapted/std_pair.hpp>
// #include <boost/fusion/adapted/std_tuple.hpp>

#include "date.h"
#include "adaptTmStruct.h"

BOOST_FUSION_ADAPT_STRUCT(
	Date,
	(bool, IsRange)
	(std::tm, Begin)
	(std::tm, End)
	(std::string, Description)
)

