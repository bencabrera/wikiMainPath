#pragma once

#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>

struct Date {
	bool IsRange;
	std::tm Begin;
	std::tm End;
	std::string Description;

	void Init();

	static std::string serialize(Date date);
	static Date deserialize(std::string str);
};

bool operator<(const std::tm& date1, const std::tm& date2);
bool operator==(const std::tm& date1, const std::tm& date2);
bool operator<(const Date& date1, const Date& date2);
bool operator==(const Date& date1, const Date& date2);
std::ostream& operator<<(std::ostream& ostr, const Date& date);

std::string to_iso_string(const std::tm& date);

Date create_date(int year, int month, int day);
Date create_date_range(int year_begin, int month_begin, int day_begin, int year_end, int month_end, int day_end);
