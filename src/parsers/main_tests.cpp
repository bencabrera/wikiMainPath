#define BOOST_TEST_MODULE "WikiMainPath Tests"
#include <boost/test/included/unit_test.hpp>

#include "articleNetwork/date.h"

BOOST_AUTO_TEST_SUITE(DateTests)

BOOST_AUTO_TEST_CASE(TestIfDateSerializationWorks)
{
	Date d1;
	d1.IsRange = false;

	d1.Begin.tm_year = 100;
	d1.Begin.tm_mon = 8;
	d1.Begin.tm_mday = 8;

	std::string str = Date::serialize(d1);
	Date d = Date::deserialize(str);
	std::string str2 = Date::serialize(d);

	BOOST_CHECK_EQUAL(str, str2);
}


BOOST_AUTO_TEST_SUITE_END()
