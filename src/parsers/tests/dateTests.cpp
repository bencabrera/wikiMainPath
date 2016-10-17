#include <boost/test/included/unit_test.hpp>

#include <fstream>
#include <streambuf>
#include <string>

#include "../articleNetwork/date.h"
#include "../articleNetwork/dateExtractor.h"

BOOST_AUTO_TEST_SUITE(DateTests)

BOOST_AUTO_TEST_CASE(TestIfDateSerializationWorks)
{
	Date d1;
	d1.IsRange = false;

	d1.Begin.tm_year = 100;
	d1.Begin.tm_mon = 8;
	d1.Begin.tm_mday = 8;

	std::string str = Date::serialize(d1);
	Date d = Date::deserialize(str);
	std::string str2 = Date::serialize(d);

	BOOST_CHECK_EQUAL(str, str2);
}

BOOST_AUTO_TEST_CASE(DateExtractionShouldWorkForExample1)
{
	DateExtractor extract_date;
	std::ifstream file("../../src/parsers/tests/data/battle_of_hastings.wikisyntax");
	std::string str((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());

	Date result_date;
	auto extraction_worked = extract_date(str, result_date);

	BOOST_CHECK_EQUAL(extraction_worked, true);

	Date correct_date;
	correct_date.IsRange = false;
	correct_date.Begin.tm_mday = 14;
	correct_date.Begin.tm_mon = 9;
	correct_date.Begin.tm_year = 1066-1900;

	BOOST_CHECK_EQUAL(result_date, correct_date);
}

BOOST_AUTO_TEST_CASE(DateExtractionShouldWorkForExample2)
{
	DateExtractor extract_date;
	std::ifstream file("../../src/parsers/tests/data/battle_of_lexington.wikisyntax");
	std::string str((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());

	Date result_date;
	auto extraction_worked = extract_date(str, result_date);

	BOOST_CHECK_EQUAL(extraction_worked, true);

	Date correct_date;
	correct_date.IsRange = false;
	correct_date.Begin.tm_mday = 19;
	correct_date.Begin.tm_mon = 3;
	correct_date.Begin.tm_year = 1775-1900;

	BOOST_CHECK_EQUAL(result_date, correct_date);
}

BOOST_AUTO_TEST_CASE(DateExtractionShouldWorkForExample3)
{
	DateExtractor extract_date;
	std::ifstream file("../../src/parsers/tests/data/storming_of_the_bastille.wikisyntax");
	std::string str((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());

	Date result_date;
	auto extraction_worked = extract_date(str, result_date);

	BOOST_CHECK_EQUAL(extraction_worked, true);

	Date correct_date;
	correct_date.IsRange = false;
	correct_date.Begin.tm_mday = 14;
	correct_date.Begin.tm_mon = 6;
	correct_date.Begin.tm_year = 1789-1900;

	BOOST_CHECK_EQUAL(result_date, correct_date);
}

BOOST_AUTO_TEST_CASE(StdDateExtractionExample1)
{
	std::string str = "14 October 1066";
	std::string format = "%d %B %Y";

	std::tm dateObj{};
	std::istringstream ss(str);
	ss.imbue(std::locale("en_US.utf-8"));
	ss >> std::get_time(&dateObj, format.c_str());	
	
	BOOST_CHECK_EQUAL(ss.fail(), false);

	BOOST_CHECK_EQUAL(dateObj.tm_mday,14);
	BOOST_CHECK_EQUAL(dateObj.tm_mon,9);
	BOOST_CHECK_EQUAL(dateObj.tm_year,1066-1900);
}

BOOST_AUTO_TEST_CASE(StdDateExtractionExample2)
{
	std::string str = "1066";
	std::string format = "%Y";

	std::tm dateObj{};
	std::istringstream ss(str);
	ss.imbue(std::locale("en_US.utf-8"));
	ss >> std::get_time(&dateObj, format.c_str());	
	
	BOOST_CHECK_EQUAL(ss.fail(), false);

	BOOST_CHECK_EQUAL(dateObj.tm_year,1066-1900);
}

BOOST_AUTO_TEST_SUITE_END()
