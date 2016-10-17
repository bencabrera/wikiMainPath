#include "date.h"
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <regex>
#include <string>
#include <iomanip>

std::tm parseDateStr(std::string str)
{
	std::tm rtn;

	std::istringstream ss(str);
	std::string token;

	std::getline(ss, token, '_');
	rtn.tm_year = std::stoi(token);
	std::getline(ss, token, '_');
	rtn.tm_mon = std::stoi(token);
	std::getline(ss, token, '_');
	rtn.tm_mday = std::stoi(token);

	return rtn;
}

Date Date::deserialize(std::string str)
{
	Date rtn;
	if(str[0] == 'r')
	{
		// range date
		rtn.IsRange = true;
		auto pos = str.find_first_of(":");
		auto str1 = str.substr(1,pos-1); 
		auto str2 = str.substr(pos+1,str.size()); 
		rtn.Begin = parseDateStr(str1);
		rtn.End = parseDateStr(str2);
	}
	else
	{
		// non-range date
		rtn.IsRange = false;
		str = str.substr(1,str.size()-1);	
		rtn.Begin = parseDateStr(str);
	}

	return rtn;
}


void Date::Init()
{
	IsRange = false;
	Begin = std::tm{};
	End = std::tm{};
}

std::string Date::serialize(Date date)
{
	std::ostringstream ss;
	ss << (date.IsRange ? "r" : "n");
	ss << date.Begin.tm_year << "_" << date.Begin.tm_mon << "_" << date.Begin.tm_mday;
	if(date.IsRange)
	ss << ":" << date.End.tm_year << "_" << date.End.tm_mon << "_" << date.End.tm_mday;

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

bool operator==(const Date& date1, const Date& date2)
{
	return !(date1 < date2) && !(date2 < date1);
}

std::ostream& operator<<(std::ostream& ostr, const Date& date)
{
	auto str = Date::serialize(date);
	ostr << str;

	return ostr;
}
