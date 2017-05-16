#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include <boost/algorithm/string/trim.hpp>

#include "../../../core/date.h"
#include "../../date/grammars/dateStringGrammar.hpp"

BOOST_AUTO_TEST_SUITE(date_string_grammar_tests)

std::vector<std::string> date_examples = {
	"1 September 1917",
	"29 november 1776",
	"18/19 December 1999 (aged 73)",
	"11-May 1944",
	"9 July – 17 August 1943",
	"9 - 17 August 1943",
	"9 – 17 August 1943",
	"9–17 August 1943",
	"29 April – 1 May 1794",
	"13–14 April 1796",
	"20 to 21 April 1797 (2 floréal an V)",
	"09 April 1991 to 22 April 1995",
	"12, April, 1750",
	"April 29, 1829 (aged 79)",
	"31 March − 1 April 1942",
	"December 15th, 2004",
	"May 25–27, 1813",
	"November 7–9, 1814",
	"August 4 – September 21, 1814"
};

std::vector<std::string> not_parse_date_examples = {
	"18 November"
};

std::vector<Date> expected_dates = {
	// tm_sec	tm_min	tm_hour	tm_mday	tm_mon	tm_year	tm_wday	tm_yday	tm_isdst
	{false, {	   0,	   0,		 0,		1,		8,	  17 }, {}},
	{false, {	   0,	   0,		 0,		29,		10,	  -124 }, {}},
	{false, {	   0,	   0,		 0,		18,		11,	  99 }, {}},
	{false, {	   0,	   0,		 0,		11,		4,	  44 }, {}},
	{true, {	   0,	   0,		 0,		9,		6,	  43 }, {	   0,	   0,		 0,		17,		7,	  43 }},
	{true, {	   0,	   0,		 0,		9,		7,	  43 }, {	   0,	   0,		 0,		17,		7,	  43 }},
	{true, {	   0,	   0,		 0,		9,		7,	  43 }, {	   0,	   0,		 0,		17,		7,	  43 }},
	{true, {	   0,	   0,		 0,		9,		7,	  43 }, {	   0,	   0,		 0,		17,		7,	  43 }},
	{true, {	   0,	   0,		 0,		29,		3,	  -106 }, {	   0,	   0,		 0,		1,		4,	  -106 }},
	{true, {	   0,	   0,		 0,		13,		3,	  -104 }, {	   0,	   0,		 0,		14,		3,	  -104 }},
	{true, {	   0,	   0,		 0,		20,		3,	  -103 }, {	   0,	   0,		 0,		21,		3,	  -103 }},
	{true, {	   0,	   0,		 0,		9,		3,	  91 }, {	   0,	   0,		 0,		22,		3,	  95 }},
	{false, {	   0,	   0,		 0,		12,		3,	  -150 }, {}},
	{false, {	   0,	   0,		 0,		29,		3,	  -71 }, {}},
	{true, {	   0,	   0,		 0,		31,		2,	  42 }, {	   0,	   0,		 0,		1,		3,	  42 }},
	{false, {	   0,	   0,		 0,		15,		11,	  104 }, {}},
	{true, {	   0,	   0,		 0,		25,		4,	  -87 }, {	   0,	   0,		 0,		27,		4,	  -87 }},
	{true, {	   0,	   0,		 0,		7,		10,	  -86 }, {	   0,	   0,		 0,		9,		10,	  -86 }},
	{true, {	   0,	   0,		 0,		4,		7,	  -86 }, {	   0,	   0,		 0,		21,		8,	  -86 }},
};

BOOST_DATA_TEST_CASE(
	date_string_grammar_should_parse_correctly,
	boost::unit_test::data::make(date_examples) ^ boost::unit_test::data::make(expected_dates),date_str, expected_date
)
{
	std::string str = date_str;

	WikiMainPath::DateStringGrammar<std::string::const_iterator, boost::spirit::qi::blank_type> date_string_grammar;

	std::pair<bool, Date> p;
	auto it = str.cbegin();
	boost::spirit::qi::phrase_parse(it, str.cend(), date_string_grammar, boost::spirit::qi::blank, p);
	bool was_extracted = p.first;
	BOOST_CHECK(was_extracted);
	BOOST_CHECK_EQUAL(expected_date, p.second);
}


BOOST_DATA_TEST_CASE(
	should_not_parse,
	boost::unit_test::data::make(not_parse_date_examples),
	date_str
)
{
	std::string str = date_str;

	WikiMainPath::DateStringGrammar<std::string::const_iterator, boost::spirit::qi::blank_type> date_string_grammar;

	std::pair<bool, Date> p;
	auto it = str.cbegin();
	boost::spirit::qi::phrase_parse(it, str.cend(), date_string_grammar, boost::spirit::qi::blank, p);
	bool was_extracted = p.first;
	BOOST_CHECK(!was_extracted);
}

BOOST_AUTO_TEST_SUITE_END()
