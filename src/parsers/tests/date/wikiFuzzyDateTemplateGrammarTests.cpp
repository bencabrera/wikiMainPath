#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include <boost/algorithm/string/trim.hpp>

#include "../../../core/date.h"
#include "../../date/grammars/wikiFuzzyDateTemplateGrammar.hpp"

BOOST_AUTO_TEST_SUITE(wiki_fuzzy_date_template_grammar_tests)

std::vector<std::string> date_template_examples = {
	"{{start-date|7 December 1941}}",
	"{{birth-date|7 December 1941}}",
	"{{death-date|7 December 1941}}",
	"{{end-date|7 December 1941}}",
	"{{birth-date|df=yes|10 November 1940}}",
	"{{death-date and age|30 April 1899|1 February 1824}}",
	"{{death-date and age|January 14, 1885|December 14, 1816}}",
	"{{Birth year and age|1965}}",
	"{{Birth year and age|df=yes|1936}}",
};

std::vector<Date> expected_template_dates = {
	// tm_sec	tm_min	tm_hour	tm_mday	tm_mon	tm_year	tm_wday	tm_yday	tm_isdst
	{false, {	   0,	   0,		 0,		7,		11,	  41 }, {}},
	{false, {	   0,	   0,		 0,		7,		11,	  41 }, {}},
	{false, {	   0,	   0,		 0,		7,		11,	  41 }, {}},
	{false, {	   0,	   0,		 0,		7,		11,	  41 }, {}},
	{false, {	   0,	   0,		 0,		10,		10,	  40 }, {}},
	{true, {	   0,	   0,		 0,		1,		1,	  -76 }, {	   0,	   0,		 0,		30,		3,	  -1 }},
	{true, {	   0,	   0,		 0,		14,		11,	  -84 }, {	   0,	   0,		 0,		14,		0,	  -15 }},
	{false, {	   0,	   0,		 0,		15,		6,	  65 }, {}},
	{false, {	   0,	   0,		 0,		15,		6,	  36 }, {}},
};

BOOST_DATA_TEST_CASE(
	fuzzy_templates_should_be_parsed_correctly,
	boost::unit_test::data::make(date_template_examples) ^ boost::unit_test::data::make(expected_template_dates),date_str, expected_date
)
{
	std::string str = date_str;

	WikiMainPath::WikiFuzzyDateTemplateGrammar<std::string::const_iterator, boost::spirit::qi::blank_type> wiki_fuzzy_date_template_grammar;
	std::pair<bool, Date> p;
	auto it = str.cbegin();
	boost::spirit::qi::phrase_parse(it, str.cend(), wiki_fuzzy_date_template_grammar, boost::spirit::qi::blank, p);
	bool was_extracted = p.first;
	BOOST_CHECK(was_extracted);
	BOOST_CHECK_EQUAL(expected_date, p.second);
}

BOOST_AUTO_TEST_SUITE_END()
