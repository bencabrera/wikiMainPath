#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <boost/algorithm/string/trim.hpp>

#include <fstream>
#include <streambuf>
#include <string>

#include "../../../core/date.h"
#include "../../date/dateExtraction.h"
#include "../../date/dateStringGrammar.hpp"
#include "../../date/infoboxGrammar.hpp"
#include "../../date/infoboxKeyValueGrammar.hpp"

BOOST_AUTO_TEST_SUITE(date_extraction_tests)


BOOST_AUTO_TEST_CASE(choosing_of_dates_should_use_hirachy)
{
	Date d1,d2,d3,d4;
	d1.Description = "GENERAL_DATE";
	d2.Description = "DEATH";
	d3.Description = "BIRTH_DEATH";
	d4.Description = "BIRTH";
	std::vector<Date> vec = { d1, d2, d3, d4 };

	Date result;	
	WikiMainPath::chooseSingleDate(vec, result);

	BOOST_CHECK_EQUAL("BIRTH_DEATH", result.Description);
}

// BOOST_AUTO_TEST_CASE(empty_date_str_should_not_be_accepted)
// {
	// std::string empty_str = "";
	// auto it = empty_str.cbegin();
	
	// WikiMainPath::DateStringGrammar<std::string::const_iterator, boost::spirit::qi::blank_type> date_grammar;
	// Date d;
	// boost::spirit::qi::phrase_parse(it, empty_str.cend(), date_grammar, boost::spirit::qi::blank, d);
// }

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

BOOST_AUTO_TEST_SUITE_END()
