#define BOOST_TEST_STATIC_LINK

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include <boost/algorithm/string/trim.hpp>

#include "../../../core/date.h"
#include "../../date/grammars/plainYearGrammar.hpp"

namespace std {
	std::ostream& operator<<(std::ostream& ostr, const std::pair<bool,Date>& p)
	{
		ostr << p.first << ", " << p.second;

		return ostr;
	}
}

BOOST_AUTO_TEST_SUITE(plain_year_grammar_tests)

	std::vector<std::string> date_examples = {
		"1917",
		"1776",
		"21776",
		"AD 100",
		"AD 150",
		"100 BC",
		"1 BC",
	};

std::vector<std::pair<bool,Date>> expected_dates = {
	// tm_sec	tm_min	tm_hour	tm_mday	tm_mon	tm_year	tm_wday	tm_yday	tm_isdst
	{true, {false, {	   0,	   0,		 0,		1,		8,	  17 }, {}}},
	{true, {false, {	   0,	   0,		 0,		29,		10,	  -124 }, {}}},
	{false, Date()},
	{true, {false, {	   0,	   0,		 0,		29,		10,	  -2000 }, {}}},
	{true, {false, {	   0,	   0,		 0,		29,		10,	  -2050 }, {}}},
	{true, {false, {	   0,	   0,		 0,		29,		10,	  -2000 }, {}}},
	{true, {false, {	   0,	   0,		 0,		29,		10,	  -1901 }, {}}},
};


BOOST_DATA_TEST_CASE(
		plain_year_grammar_should_parse_correctly,
		boost::unit_test::data::make(date_examples) ^ boost::unit_test::data::make(expected_dates),date_str, expected
		)
{
	std::string str = date_str;

	WikiMainPath::PlainYearGrammar<std::string::const_iterator, boost::spirit::qi::blank_type> date_string_grammar;

	std::pair<bool, Date> p;
	auto it = str.cbegin();
	boost::spirit::qi::phrase_parse(it, str.cend(), date_string_grammar, boost::spirit::qi::blank, p);
	bool was_extracted = p.first;

	bool expected_extracted = expected.first;
	auto expected_date = expected.second;

	BOOST_CHECK_EQUAL(expected_extracted, was_extracted);
	if(expected_extracted)
		BOOST_CHECK_EQUAL(expected_date.Begin.tm_year, p.second.Begin.tm_year);
}

BOOST_AUTO_TEST_SUITE_END()
