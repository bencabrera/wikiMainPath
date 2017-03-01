#define BOOST_TEST_DYN_LINK

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
};

std::vector<Date> expected_template_dates = {
	// tm_sec	tm_min	tm_hour	tm_mday	tm_mon	tm_year	tm_wday	tm_yday	tm_isdst
	{false, {	   0,	   0,		 0,		7,		11,	  41 }, {}},
	{false, {	   0,	   0,		 0,		7,		11,	  41 }, {}},
	{false, {	   0,	   0,		 0,		7,		11,	  41 }, {}},
	{false, {	   0,	   0,		 0,		7,		11,	  41 }, {}},
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
