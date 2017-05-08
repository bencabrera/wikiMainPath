#include "date.h"

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <regex>
#include <string>
#include <iomanip>

// spirit
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

#include "adaptTmStruct.h"
#include "adaptDate.h"

void Date::Init()
{
	IsRange = false;
	Begin = std::tm{};
	End = std::tm{};
}

std::string Date::serialize(Date date)
{
	std::ostringstream ss;
	ss << (date.IsRange ? "r" : "");
	ss << (date.Begin.tm_year + 1900) << "_" << (date.Begin.tm_mon + 1) << "_" << date.Begin.tm_mday;

	if(date.IsRange)
		ss << ":" << (date.End.tm_year + 1900) << "_" << (date.End.tm_mon + 1) << "_" << date.End.tm_mday;

	if(date.Description != "")
		ss << ":" << date.Description;

	return ss.str();
}

Date Date::deserialize(const std::string str)
{
	using namespace boost::spirit::qi;
	using namespace boost::phoenix;
	using boost::phoenix::val;
	using boost::phoenix::construct;

	Date rtn;

	boost::spirit::qi::rule<std::string::const_iterator, Date()> dateRule;
	boost::spirit::qi::rule<std::string::const_iterator, std::tm()> tmDateRule;

	dateRule = ((lit("r") [at_c<0>(_val) = true] >> tmDateRule [at_c<1>(_val) = boost::spirit::qi::_1] >> tmDateRule [at_c<2>(_val) = boost::spirit::qi::_1])
				| (eps [at_c<0>(_val) = false] >> tmDateRule [at_c<1>(_val) = boost::spirit::qi::_1]))
				>> -(':' >> +(char_ [at_c<3>(_val) += boost::spirit::qi::_1]));
	tmDateRule = int_ [at_c<0>(_val) = boost::spirit::qi::_1 - 1900] >> '_' >> int_ [at_c<1>(_val) = boost::spirit::qi::_1 - 1] >> '_' >> int_ [at_c<2>(_val) = boost::spirit::qi::_1];
	
	boost::spirit::qi::parse(str.cbegin(), str.cend(), dateRule, rtn);

	return rtn;
}

std::string to_iso_string(const std::tm& date)
{
	std::stringstream ss;
	ss << std::setfill('0') << std::setw(4) << (date.tm_year + 1900) << "-" << std::setfill('0') << std::setw(2) << (date.tm_mon + 1) << "-" << std::setfill('0') << std::setw(2) << date.tm_mday;
	ss << "T";
	ss << "12:00Z";

	return ss.str();
}

bool operator<(const std::tm& date1, const std::tm& date2)
{
	if(date1.tm_year != date2.tm_year)
		return date1.tm_year < date2.tm_year;
	else if(date1.tm_mon != date2.tm_mon)
		return date1.tm_mon < date2.tm_mon;
	else 
		return date1.tm_mday < date2.tm_mday;
}

bool operator<(const Date& date1, const Date& date2)
{
	if(!date1.IsRange && !date2.IsRange)
		return date1.Begin < date2.Begin;
	//if(!date1.IsRange && date2.IsRange)
	//{
	//	if(date1.Begin < date2.Begin)
	//		return true;
	//	if(!(date1.Begin < date2.End))
	//		return false;
	//}

	throw std::logic_error("Not implemented.");
}

bool operator==(const std::tm& date1, const std::tm& date2)
{
	if(date1.tm_sec != date2.tm_sec)
		return false;
	if(date1.tm_min != date2.tm_min)
		return false;
	if(date1.tm_mday != date2.tm_mday)
		return false;
	if(date1.tm_mon != date2.tm_mon)
		return false;
	if(date1.tm_year != date2.tm_year)
		return false;

	return true;
}

bool operator==(const Date& date1, const Date& date2)
{
	if(date1.IsRange != date2.IsRange)
		return false;
	if(!(date1.Begin == date2.Begin))
		return false;
	if(date1.IsRange)
	{
		if(!(date1.End == date2.End))
			return false;
	}

	return true;
}

std::ostream& operator<<(std::ostream& ostr, const Date& date)
{
	auto str = Date::serialize(date);
	ostr << str;

	return ostr;
}


Date create_date(int year, int month, int day)
{
	Date d;
	d.Init();
	d.Begin.tm_year = year - 1900;
	d.Begin.tm_mon= month - 1;
	d.Begin.tm_mday= day;

	return d;
}

Date create_date_range(int year_begin, int month_begin, int day_begin, int year_end, int month_end, int day_end)
{
	Date d;
	d.Init();

	d.IsRange = true;
	d.Begin.tm_year = year_begin - 1900;
	d.Begin.tm_mon= month_begin - 1;
	d.Begin.tm_mday= day_begin;

	d.End.tm_year = year_end - 1900;
	d.End.tm_mon = month_end - 1;
	d.End.tm_mday = day_end;

	return d;
}
