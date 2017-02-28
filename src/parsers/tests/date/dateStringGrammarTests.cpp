#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include <boost/algorithm/string/trim.hpp>

#include "../../../core/date.h"
#include "../../date/infoboxDateExtraction.h"

BOOST_AUTO_TEST_SUITE(date_string_grammar_tests)

std::vector<std::string> date_examples = {
	"1 September 1917",
};

std::vector<Date> expected_dates = {
	// tm_sec	tm_min	tm_hour	tm_mday	tm_mon	tm_year	tm_wday	tm_yday	tm_isdst
	{false, {	   0,	   0,		 0,		1,		8,	  17 }, {}},
};

BOOST_DATA_TEST_CASE(
	date_string_grammar_should_parse_correctly,
	boost::unit_test::data::make(date_examples) ^ boost::unit_test::data::make(expected_dates),date_str, expected_date
)
{
	std::string str = date_str;
	Date d;
	auto date_was_extracted = WikiMainPath::extractDateFromString(str, d);
	BOOST_CHECK(date_was_extracted);
	BOOST_CHECK_EQUAL(expected_date, d);
}

BOOST_AUTO_TEST_SUITE_END()
