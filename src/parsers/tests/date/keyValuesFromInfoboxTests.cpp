#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include <boost/algorithm/string/trim.hpp>

#include <fstream>
#include <streambuf>
#include <string>

#include "../../../core/date.h"
#include "../helpers/fileListDataset.hpp"
#include "../../date/infoboxDateExtraction.h"

BOOST_AUTO_TEST_SUITE(key_value_from_infobox_tests)

BOOST_DATA_TEST_CASE(
	extract_all_correct_key_values_from_infobox,
	WikiMainPath::Tests::FileListDataset("../../src/parsers/tests/date/data", "INFOBOX_SYNTAX", ".wikisyntax")
	^ boost::unit_test::data::make(1)
	^ WikiMainPath::Tests::FileListDataset("../../src/parsers/tests/date/data", "INFOBOX_KEY_VALUES", ".txt"),
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

BOOST_AUTO_TEST_SUITE_END()
