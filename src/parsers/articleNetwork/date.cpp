#include "date.h"
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <regex>
#include <string>
#include <iomanip>


Date Date::deserialize(std::string str)
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
