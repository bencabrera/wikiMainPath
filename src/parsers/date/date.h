#pragma once

#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
// #include <boost/fusion/include/io.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>

#include "adaptTmStruct.h"

struct Date {
	bool IsRange;
	std::tm Begin;
	std::tm End;
	std::string Description;

	void Init();

	static std::string serialize(Date date);
	static Date deserialize(std::string str);
};

BOOST_FUSION_ADAPT_STRUCT(
	Date,
	(bool, IsRange)
	(std::tm, Begin)
	(std::tm, End)
	(std::string, Description)
)

bool operator<(const std::tm& date1, const std::tm& date2);
bool operator==(const std::tm& date1, const std::tm& date2);
bool operator<(const Date& date1, const Date& date2);
bool operator==(const Date& date1, const Date& date2);
std::ostream& operator<<(std::ostream& ostr, const Date& date);
