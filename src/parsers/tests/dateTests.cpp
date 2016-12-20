#pragma once
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include <fstream>
#include <streambuf>
#include <string>

#include "../date/date.h"
#include "../date/dateExtractor.h"
#include "../date/dateStringGrammar.hpp"

BOOST_AUTO_TEST_SUITE(DateTests)

BOOST_AUTO_TEST_CASE(TestIfDateSerializationWorks)
{
	Date d1;
	d1.IsRange = false;

	d1.Begin.tm_year = 100;
	d1.Begin.tm_mon = 8;
	d1.Begin.tm_mday = 8;
	d1.Description = "Test";

	std::string str = Date::serialize(d1);
	Date d = Date::deserialize(str);
	std::string str2 = Date::serialize(d);

	BOOST_CHECK_EQUAL(str, str2);
}


std::vector<std::string> date_examples = {
	"{{Age|1989|7|23|2003|7|14}}",
	"{{Age nts|1989|7|23}}",
	"{{Age nts|1989|7|23|2003|7|14}}",
	"1 September 1917"
};

std::vector<Date> expected_dates = {
	// tm_sec	tm_min	tm_hour	tm_mday	tm_mon	tm_year	tm_wday	tm_yday	tm_isdst
	{true, {	   0,	   0,		 0,		23,		6,	  89 }, {	   0,	   0,		 0,		14,		6,	  103 }},
	{false, {	   0,	   0,		 0,		23,		6,	  89 }, {}},
	{true, {	   0,	   0,		 0,		23,		6,	  89 }, {	   0,	   0,		 0,		14,		6,	  103 }},
	{false, {	   0,	   0,		 0,		1,		8,	  17 }, {}}
};

BOOST_DATA_TEST_CASE(should_run,boost::unit_test::data::make(date_examples),date_str)
{
	std::string str = date_str;
	auto it = str.begin();
	boost::spirit::qi::phrase_parse(it, str.end(), WikiMainPath::DateStringGrammar<std::string::iterator, boost::spirit::qi::blank_type>(), boost::spirit::qi::blank);
	BOOST_CHECK(it == str.end());
}

BOOST_DATA_TEST_CASE(should_run2,boost::unit_test::data::make(date_examples) ^ boost::unit_test::data::make(expected_dates),date_str, expected_date)
{
	std::string str = date_str;
	auto it = str.begin();
	Date d;
	d.Init();
	boost::spirit::qi::phrase_parse(it, str.end(), WikiMainPath::DateStringGrammar<std::string::iterator, boost::spirit::qi::blank_type>(), boost::spirit::qi::blank, d);
	BOOST_CHECK_EQUAL(expected_date, d);
}


BOOST_AUTO_TEST_CASE(DateExtractionShouldWorkForExample1)
{
	using namespace boost::spirit::qi;
	boost::spirit::qi::symbols<char> sym;
	
	sym = "Age", "Age nts", "age for infant";

	std::string str = "{{Age";
	auto it = str.begin();
	boost::spirit::qi::phrase_parse(it, str.end(), lit("{{") >> sym, boost::spirit::qi::blank);
	BOOST_CHECK(it == str.end());
}

// BOOST_AUTO_TEST_CASE(DateExtractionShouldWorkForExample1)
// {
// DateExtractor extract_date;
// std::ifstream file("../../src/parsers/tests/data/battle_of_hastings.wikisyntax");
// std::string str((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());

// Date result_date;
// auto extraction_worked = extract_date(str, result_date);

// BOOST_CHECK_EQUAL(extraction_worked, true);

// Date correct_date;
// correct_date.IsRange = false;
// correct_date.Begin.tm_mday = 14;
// correct_date.Begin.tm_mon = 9;
// correct_date.Begin.tm_year = 1066-1900;

// BOOST_CHECK_EQUAL(result_date, correct_date);
// }

// BOOST_AUTO_TEST_CASE(DateExtractionShouldWorkForExample2)
// {
// DateExtractor extract_date;
// std::ifstream file("../../src/parsers/tests/data/battle_of_lexington.wikisyntax");
// std::string str((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());

// Date result_date;
// auto extraction_worked = extract_date(str, result_date);

// BOOST_CHECK_EQUAL(extraction_worked, true);

// Date correct_date;
// correct_date.IsRange = false;
// correct_date.Begin.tm_mday = 19;
// correct_date.Begin.tm_mon = 3;
// correct_date.Begin.tm_year = 1775-1900;

// BOOST_CHECK_EQUAL(result_date, correct_date);
// }

// BOOST_AUTO_TEST_CASE(DateExtractionShouldWorkForExample3)
// {
// DateExtractor extract_date;
// std::ifstream file("../../src/parsers/tests/data/storming_of_the_bastille.wikisyntax");
// std::string str((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());

// Date result_date;
// auto extraction_worked = extract_date(str, result_date);

// BOOST_CHECK_EQUAL(extraction_worked, true);

// Date correct_date;
// correct_date.IsRange = false;
// correct_date.Begin.tm_mday = 14;
// correct_date.Begin.tm_mon = 6;
// correct_date.Begin.tm_year = 1789-1900;

// BOOST_CHECK_EQUAL(result_date, correct_date);
// }

// BOOST_AUTO_TEST_CASE(DateExtractionShouldWorkForExample4)
// {
// DateExtractor extract_date;
// std::ifstream file("../../src/parsers/tests/data/september_11.wikisyntax");
// std::string str((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());

// Date result_date;
// auto extraction_worked = extract_date(str, result_date);

// BOOST_CHECK_EQUAL(extraction_worked, true);

// Date correct_date;
// correct_date.IsRange = false;
// correct_date.Begin.tm_mday = 11;
// correct_date.Begin.tm_mon = 8;
// correct_date.Begin.tm_year = 2001-1900;

// BOOST_CHECK_EQUAL(result_date, correct_date);
// }

// BOOST_AUTO_TEST_CASE(DateExtractionShouldWorkForExample5)
// {
// DateExtractor extract_date;
// std::ifstream file("../../src/parsers/tests/data/battle_of_los_angeles.wikisyntax");
// std::string str((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());

// Date result_date;
// auto extraction_worked = extract_date(str, result_date);

// BOOST_CHECK_EQUAL(extraction_worked, true);

// Date correct_date;
// correct_date.IsRange = true;
// correct_date.Begin.tm_mday = 24;
// correct_date.Begin.tm_mon = 1;
// correct_date.Begin.tm_year = 1942-1900;
// correct_date.End.tm_mday = 25;
// correct_date.End.tm_mon = 1;
// correct_date.End.tm_year = 1942-1900;

// BOOST_CHECK_EQUAL(result_date, correct_date);
// }

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
