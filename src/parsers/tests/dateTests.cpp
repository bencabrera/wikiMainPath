#pragma once
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include <boost/algorithm/string/trim.hpp>

#include <fstream>
#include <streambuf>
#include <string>

#include "../date/date.h"
#include "../date/dateExtraction.h"
#include "../date/dateStringGrammar.hpp"
#include "fileListDataset.hpp"
#include "../date/infoboxGrammar.hpp"
#include "../date/infoboxKeyValueGrammar.hpp"

BOOST_AUTO_TEST_SUITE(DateTests)

// BOOST_DATA_TEST_CASE(
	// extract_all_correct_dates_from_infobox,
	// WikiMainPath::Tests::FileListDataset("../../src/parsers/tests/dateExtractionTestData", "ARTICLE", ".wikisyntax")
	// ^ boost::unit_test::data::make(1)
	// ^ WikiMainPath::Tests::FileListDataset("../../src/parsers/tests/dateExtractionTestData", "DATE_LIST", ".txt"),
	// article_path, dummy, date_list_path
// )
// {
	// using namespace WikiMainPath;
	// using namespace WikiMainPath::Tests;

	// // --- parsing the annotations ---
	// std::ifstream dateListFile(date_list_path.string());
	// std::map<std::string, Date> expected_dates;
	// while(!dateListFile.eof()) {
		// std::string line, dateLabel, dateStr;
		// std::getline(dateListFile, line);
		// boost::trim(line);
		// if(line != "")
		// {
			// std::cout << "LINE: [" << line << "]" << std::endl;
			// std::stringstream ss_line(line);
			// std::getline(ss_line, dateLabel, '\t');
			// std::getline(ss_line, dateStr, '\n');
			// expected_dates.insert({ dateLabel, Date::deserialize(dateStr) });	
		// }
	// }

	// std::ifstream articleFile(article_path.string());
	// std::string articleSyntax((std::istream_iterator<char>(articleFile)), std::istream_iterator<char>());
	// auto parsed_dates = extractAllDatesFromInfobox(articleSyntax);

	// std::cout << "------ Expected ------" << std::endl;
	// for (auto i : expected_dates) {
		// std::cout << i.first << " --- " << Date::serialize(i.second) << std::endl;
	// }

	// std::cout << "------ Parsed ------" << std::endl;
	// for (auto i : parsed_dates) {
		// std::cout << i.first << " --- " << Date::serialize(i.second) << std::endl;
	// }

	// for (auto exp_date : expected_dates) {
		// auto it = parsed_dates.find(exp_date.first);
		// BOOST_CHECK(it != parsed_dates.end()); 			// date does exists
		// BOOST_CHECK_EQUAL(it->second, exp_date.second); // date is equal
	// }
// }


BOOST_DATA_TEST_CASE(
	extract_all_correct_key_values_from_infobox,
	WikiMainPath::Tests::FileListDataset("../../src/parsers/tests/dateExtractionTestData", "INFOBOX_SYNTAX", ".wikisyntax")
	^ boost::unit_test::data::make(1)
	^ WikiMainPath::Tests::FileListDataset("../../src/parsers/tests/dateExtractionTestData", "INFOBOX_KEY_VALUES", ".txt"),
	infobox_syntax_path, dummy, expected_key_values_path
)
{
	using namespace WikiMainPath;
	using namespace WikiMainPath::Tests;

	// --- parsing the annotations ---
	std::ifstream expected_key_values_file(expected_key_values_path.string());
	std::vector<std::pair<std::string, std::string>> expected_key_values;
	while(!expected_key_values_file.eof()) {
		std::string line, key, value;
		std::getline(expected_key_values_file, line);
		boost::trim(line);
		if(line != "")
		{
			std::stringstream ss_line(line);
			std::getline(ss_line, key, '\t');
			std::getline(ss_line, value, '\n');
			boost::trim(key);
			boost::trim(value);
			expected_key_values.push_back({ key, value });	
		}
	}

	std::ifstream infobox_syntax_file(infobox_syntax_path.string());
	std::stringstream buffer;
	buffer << infobox_syntax_file.rdbuf();
	auto parsed_key_values = extractAllKeyValuesFromInfobox(buffer.str());

	// std::cout << "------ Expected ------" << std::endl;
	// for (auto i : expected_key_values) {
		// std::cout << i.first << " --- " << i.second << std::endl;
	// }

	// std::cout << "------ Parsed ------" << std::endl;
	// for (auto i : parsed_key_values) {
		// std::cout << i.first << " --- " << i.second << std::endl;
	// }

	BOOST_REQUIRE_EQUAL(expected_key_values.size(), parsed_key_values.size());
	for(std::size_t i = 0; i < parsed_key_values.size(); i++)
	{
		auto& parsed_val = parsed_key_values[i].second;
		auto& expected_val = expected_key_values[i].second;
		parsed_val.erase(std::remove(parsed_val.begin(), parsed_val.end(), '\n'), parsed_val.end());
		parsed_val.erase(std::remove(parsed_val.begin(), parsed_val.end(), ' '), parsed_val.end());
		expected_val.erase(std::remove(expected_val.begin(), expected_val.end(), '\n'), expected_val.end());
		expected_val.erase(std::remove(expected_val.begin(), expected_val.end(), ' '), expected_val.end());
		BOOST_CHECK_EQUAL(expected_key_values[i].first, parsed_key_values[i].first);
		BOOST_CHECK_EQUAL(expected_key_values[i].second, parsed_key_values[i].second);
	}
}

// BOOST_DATA_TEST_CASE(
	// extract_all_correct_key_values_from_article,
	// WikiMainPath::Tests::FileListDataset("../../src/parsers/tests/dateExtractionTestData", "ARTICLE", ".wikisyntax")
	// ^ boost::unit_test::data::make(1)
	// ^ WikiMainPath::Tests::FileListDataset("../../src/parsers/tests/dateExtractionTestData", "INFOBOX_KEY_VALUES", ".txt"),
	// infobox_syntax_path, dummy, expected_key_values_path
// )
// {
	// using namespace WikiMainPath;
	// using namespace WikiMainPath::Tests;

	// // --- parsing the annotations ---
	// std::ifstream expected_key_values_file(expected_key_values_path.string());
	// std::vector<std::pair<std::string, std::string>> expected_key_values;
	// while(!expected_key_values_file.eof()) {
		// std::string line, key, value;
		// std::getline(expected_key_values_file, line);
		// boost::trim(line);
		// if(line != "")
		// {
			// std::stringstream ss_line(line);
			// std::getline(ss_line, key, '\t');
			// std::getline(ss_line, value, '\n');
			// boost::trim(key);
			// boost::trim(value);
			// expected_key_values.push_back({ key, value });	
		// }
	// }

	// std::ifstream infobox_syntax_file(infobox_syntax_path.string());
	// std::stringstream buffer;
	// buffer << infobox_syntax_file.rdbuf();
	// auto parsed_key_values = extractAllKeyValuesFromInfobox(buffer.str());

	// // std::cout << "------ Expected ------" << std::endl;
	// // for (auto i : expected_key_values) {
		// // std::cout << i.first << " --- " << i.second << std::endl;
	// // }

	// // std::cout << "------ Parsed ------" << std::endl;
	// // for (auto i : parsed_key_values) {
		// // std::cout << i.first << " --- " << i.second << std::endl;
	// // }

	// BOOST_REQUIRE_EQUAL(expected_key_values.size(), parsed_key_values.size());
	// for(std::size_t i = 0; i < parsed_key_values.size(); i++)
	// {
		// auto& parsed_val = parsed_key_values[i].second;
		// parsed_val.erase(std::remove(parsed_val.begin(), parsed_val.end(), '\n'), parsed_val.end());
		// parsed_val.erase(std::remove(parsed_val.begin(), parsed_val.end(), ' '), parsed_val.end());
		// BOOST_CHECK_EQUAL(expected_key_values[i].first, parsed_key_values[i].first);
		// BOOST_CHECK_EQUAL(expected_key_values[i].second, parsed_key_values[i].second);
	// }
// }

// BOOST_AUTO_TEST_CASE(date_serialization_works)
// {
	// Date d1;
	// d1.IsRange = false;

	// d1.Begin.tm_year = 100;
	// d1.Begin.tm_mon = 8;
	// d1.Begin.tm_mday = 8;
	// d1.Description = "Test";

	// std::string str = Date::serialize(d1);
	// Date d = Date::deserialize(str);
	// std::string str2 = Date::serialize(d);

	// BOOST_CHECK_EQUAL(str, str2);
// }

// BOOST_AUTO_TEST_CASE(infobox_start_should_work)
// {
	// std::string test_str = "{{Infobox civilian attack\n";
	// auto it = test_str.cbegin();

	// WikiMainPath::InfoboxGrammar<std::string::const_iterator, boost::spirit::qi::blank_type> obj;

	// boost::spirit::qi::phrase_parse(it, test_str.cend(), obj.start_infobox, boost::spirit::qi::blank);
	// BOOST_CHECK(it == test_str.cend());
// }


// BOOST_AUTO_TEST_CASE(infobox_date_with_label_should_work)
// {
	// std::string test_str = "| date        = {{start date and age|2016|12|19}}";
	// auto it = test_str.cbegin();

	// WikiMainPath::InfoboxGrammar<std::string::const_iterator, boost::spirit::qi::blank_type> obj;

	// std::pair<std::string, Date> parsed, expected({ "date", {false, {	   0,	   0,		 0,		19,		11,	  116 }, {}} });
	// boost::spirit::qi::phrase_parse(it, test_str.cend(), obj.infobox_line_with_date, boost::spirit::qi::blank, parsed);

	// BOOST_CHECK(it == test_str.cend());
	// BOOST_CHECK_EQUAL(expected.first, boost::trim_copy(parsed.first));
	// BOOST_CHECK_EQUAL(expected.second, parsed.second);
// }

// BOOST_AUTO_TEST_CASE(infobox_should_parse)
// {
	std::string test_str = "{{Infobox civilian attack \n\
	| title       = 2016 Berlin attack \n\
	| date        = {{start date and age|df=yes|paren=yes|2016|12|19}}\n\
	| time        = 20:02 [[Central European Time|CET]] ([[UTC+01]])\n\
	| type        = [[Vehicular assault]], [[truck hijacking]], [[stabbing]], [[shooting]], [[mass murder]]\n\
	| location    =  [[Breitscheidplatz]], [[Berlin]], Germany\n\
	| injuries    = 56\n\
	| fatalities  = {{#property:P1120}} (1 passenger and 11 pedestrians)\n\
	| perpetrator = \n\
	| motive      = \n\
	| assailant   = Anis Amri (deceased)\n\
	| weapons     = [[Scania PRT-range|Scania R 450]] [[semi-trailer truck]], small-caliber gun, knife}}\n\
	";

	// // auto it = test_str.cbegin();

	// // WikiMainPath::InfoboxKeyValueGrammar<std::string::const_iterator, boost::spirit::qi::blank_type> obj;

	// std::vector<std::pair<std::string, std::string>> expected_key_values = {
		// { "title", "2016 Berlin attack" },
		// { "date", "{{start date and age|df=yes|paren=yes|2016|12|19}}" },
		// { "time", "20:02 [[Central European Time|CET]] ([[UTC+01]])" },
		// { "type", "[[Vehicular assault]], [[truck hijacking]], [[stabbing]], [[shooting]], [[mass murder]]" },
		// { "location", "[[Breitscheidplatz]], [[Berlin]], Germany" },
		// { "injuries", "56" },
		// { "fatalities", "{{#property:P1120}} (1 passenger and 11 pedestrians)" },
		// { "perpetrator", "" },
		// { "motive", "" },
		// { "assailant", "Anis Amri (deceased)" },
		// { "weapons", "[[Scania PRT-range|Scania R 450]] [[semi-trailer truck]], small-caliber gun, knife" }
	// };
	// // std::vector<std::pair<std::string, std::string>> parsed_key_values;
	// // boost::spirit::qi::phrase_parse(it, test_str.cend(), obj.infobox, boost::spirit::qi::blank, parsed_key_values);

	// auto parsed_key_values = WikiMainPath::extractAllKeyValuesFromInfobox(test_str);

	// // BOOST_REQUIRE(it == test_str.cend());
	// BOOST_REQUIRE_EQUAL(expected_key_values.size(), parsed_key_values.size());
	// for(std::size_t i = 0; i < parsed_key_values.size(); i++)
	// {
		// BOOST_CHECK_EQUAL(expected_key_values[i].first, parsed_key_values[i].first);
		// BOOST_CHECK_EQUAL(expected_key_values[i].second, parsed_key_values[i].second);
	// }
// }

// std::vector<std::string> date_examples = {
	// "{{Age|1989|7|23|2003|7|14}}",
	// "{{Age nts|1989|7|23}}",
	// "{{Age nts|1989|7|23|2003|7|14}}",
	// "1 September 1917",
	// "{{Birth date|1993|2|4|mf=yes}}",
	// "{{Birth date|1993|2|4}}",
	// "{{start-date|7 December 1941}}",
	// "{{birth-date|7 December 1941}}",
	// "{{death-date|7 December 1941}}",
	// "{{end-date|7 December 1941}}",
	// "{{Death date|1993|2|4|df=yes}}",
	// "{{start date and age|df=yes|paren=yes|2016|12|19}}",
	// "{{start date and age|2016|12|19}}"
// };

// std::vector<Date> expected_dates = {
	// // tm_sec	tm_min	tm_hour	tm_mday	tm_mon	tm_year	tm_wday	tm_yday	tm_isdst
	// {true, {	   0,	   0,		 0,		23,		6,	  89 }, {	   0,	   0,		 0,		14,		6,	  103 }},
	// {false, {	   0,	   0,		 0,		23,		6,	  89 }, {}},
	// {true, {	   0,	   0,		 0,		23,		6,	  89 }, {	   0,	   0,		 0,		14,		6,	  103 }},
	// {false, {	   0,	   0,		 0,		1,		8,	  17 }, {}},
	// {false, {	   0,	   0,		 0,		4,		1,	  93 }, {}},
	// {false, {	   0,	   0,		 0,		4,		1,	  93 }, {}},
	// {false, {	   0,	   0,		 0,		7,		11,	  41 }, {}},
	// {false, {	   0,	   0,		 0,		7,		11,	  41 }, {}},
	// {false, {	   0,	   0,		 0,		7,		11,	  41 }, {}},
	// {false, {	   0,	   0,		 0,		7,		11,	  41 }, {}},
	// {false, {	   0,	   0,		 0,		4,		1,	  93 }, {}},
	// {false, {	   0,	   0,		 0,		19,		11,	  116 }, {}},
	// {false, {	   0,	   0,		 0,		19,		11,	  116 }, {}}
// };

// BOOST_DATA_TEST_CASE(should_run2,boost::unit_test::data::make(date_examples) ^ boost::unit_test::data::make(expected_dates),date_str, expected_date)
// {
	// std::string str = date_str;
	// auto it = str.begin();
	// Date d;
	// d.Init();
	// boost::spirit::qi::phrase_parse(it, str.end(), WikiMainPath::DateStringGrammar<std::string::iterator, boost::spirit::qi::blank_type>(), boost::spirit::qi::blank, d);
	// BOOST_CHECK(it == str.end());
	// BOOST_CHECK_EQUAL(expected_date, d);
// }


// BOOST_AUTO_TEST_CASE(DateExtractionShouldWorkForExample1)
// {
	// using namespace boost::spirit::qi;
	// boost::spirit::qi::symbols<char> sym;
	
	// sym = "Age", "Age nts", "age for infant";

	// std::string str = "{{Age";
	// auto it = str.begin();
	// boost::spirit::qi::phrase_parse(it, str.end(), lit("{{") >> sym, boost::spirit::qi::blank);
	// BOOST_CHECK(it == str.end());
// }

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

// BOOST_AUTO_TEST_CASE(StdDateExtractionExample1)
// {
	// std::string str = "14 October 1066";
	// std::string format = "%d %B %Y";

	// std::tm dateObj{};
	// std::istringstream ss(str);
	// ss.imbue(std::locale("en_US.utf-8"));
	// ss >> std::get_time(&dateObj, format.c_str());	

	// BOOST_CHECK_EQUAL(ss.fail(), false);

	// BOOST_CHECK_EQUAL(dateObj.tm_mday,14);
	// BOOST_CHECK_EQUAL(dateObj.tm_mon,9);
	// BOOST_CHECK_EQUAL(dateObj.tm_year,1066-1900);
// }

// BOOST_AUTO_TEST_CASE(StdDateExtractionExample2)
// {
	// std::string str = "1066";
	// std::string format = "%Y";

	// std::tm dateObj{};
	// std::istringstream ss(str);
	// ss.imbue(std::locale("en_US.utf-8"));
	// ss >> std::get_time(&dateObj, format.c_str());	

	// BOOST_CHECK_EQUAL(ss.fail(), false);

	// BOOST_CHECK_EQUAL(dateObj.tm_year,1066-1900);
// }

BOOST_AUTO_TEST_SUITE_END()
