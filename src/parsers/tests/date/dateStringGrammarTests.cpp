#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include <boost/algorithm/string/trim.hpp>

#include "../../../core/date.h"
#include "../../date/dateStringGrammar.hpp"

BOOST_AUTO_TEST_SUITE(date_string_grammar_tests)

std::vector<std::string> date_examples = {
	"{{Age|1989|7|23|2003|7|14}}",
	"{{Age nts|1989|7|23}}",
	"{{Age nts|1989|7|23|2003|7|14}}",
	"1 September 1917",
	"{{Birth date|1993|2|4|mf=yes}}",
	"{{Birth date|1993|2|4}}",
	"{{start-date|7 December 1941}}",
	"{{birth-date|7 December 1941}}",
	"{{death-date|7 December 1941}}",
	"{{end-date|7 December 1941}}",
	"{{Death date|1993|2|4|df=yes}}",
	"{{start date and age|df=yes|paren=yes|2016|12|19}}",
	"{{start date and age|2016|12|19}}",

	"{{birth date|1928|12|30}}",
	"{{death date and age|2008|06|2|1928|12|30}}",
	"{{birth date|1882|10|20|df=yes}}",
	"{{death date and age|1956|08|16|1882|10|20|df=yes}}"
};

std::vector<Date> expected_dates = {
	// tm_sec	tm_min	tm_hour	tm_mday	tm_mon	tm_year	tm_wday	tm_yday	tm_isdst
	{true, {	   0,	   0,		 0,		23,		6,	  89 }, {	   0,	   0,		 0,		14,		6,	  103 }},
	{false, {	   0,	   0,		 0,		23,		6,	  89 }, {}},
	{true, {	   0,	   0,		 0,		23,		6,	  89 }, {	   0,	   0,		 0,		14,		6,	  103 }},
	{false, {	   0,	   0,		 0,		1,		8,	  17 }, {}},
	{false, {	   0,	   0,		 0,		4,		1,	  93 }, {}},
	{false, {	   0,	   0,		 0,		4,		1,	  93 }, {}},
	{false, {	   0,	   0,		 0,		7,		11,	  41 }, {}},
	{false, {	   0,	   0,		 0,		7,		11,	  41 }, {}},
	{false, {	   0,	   0,		 0,		7,		11,	  41 }, {}},
	{false, {	   0,	   0,		 0,		7,		11,	  41 }, {}},
	{false, {	   0,	   0,		 0,		4,		1,	  93 }, {}},
	{false, {	   0,	   0,		 0,		19,		11,	  116 }, {}},
	{false, {	   0,	   0,		 0,		19,		11,	  116 }, {}},

	{false, {	   0,	   0,		 0,		30,		11,	  28 }, {}},
	{true, {	   0,	   0,		 0,		30,		11,	  28 }, {	   0,	   0,		 0,		2,		5,	  108 }},
	{false, {	   0,	   0,		 0,		20,		9,	  -18 }, {}},
	{false, {	   0,	   0,		 0,		20,		9,	  -18 }, {	   0,	   0,		 0,		16,		7,	  56 }}
};

BOOST_DATA_TEST_CASE(
	date_string_grammar_should_parse_correctly,
	boost::unit_test::data::make(date_examples) ^ boost::unit_test::data::make(expected_dates),date_str, expected_date
)
{
	std::string str = date_str;
	auto it = str.cbegin();
	Date d;
	d.Init();
	boost::spirit::qi::phrase_parse(it, str.cend(), WikiMainPath::DateStringGrammar<std::string::const_iterator, boost::spirit::qi::blank_type>(), boost::spirit::qi::blank, d);
	BOOST_CHECK(it == str.end());
	BOOST_CHECK_EQUAL(expected_date, d);
}

BOOST_AUTO_TEST_SUITE_END()
