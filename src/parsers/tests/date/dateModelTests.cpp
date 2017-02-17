#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <boost/algorithm/string/trim.hpp>

#include "../../../core/date.h"

BOOST_AUTO_TEST_SUITE(date_model_tests)

BOOST_AUTO_TEST_CASE(date_serialization_works)
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

BOOST_AUTO_TEST_SUITE_END()
