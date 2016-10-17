#pragma once

#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>

class Date {
public:
	bool IsRange;
	std::tm Begin;
	std::tm End;

	void Init();

	static std::string serialize(Date date);
	static Date deserialize(std::string str);
};

bool operator<(const std::tm& date1, const std::tm& date2);
bool operator<(const Date& date1, const Date& date2);
bool operator==(const Date& date1, const Date& date2);
std::ostream& operator<<(std::ostream& ostr, const Date& date);
