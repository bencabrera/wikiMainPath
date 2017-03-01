#pragma once

#define BOOST_SPIRIT_DEBUG

#include "dateStringGrammar.hpp"

namespace WikiMainPath {

	template<typename Iterator, typename Skipper>
		DateStringGrammar<Iterator, Skipper>::DateStringGrammar() : DateStringGrammar::base_type(start, "DateStringGrammar") {
			using namespace boost::spirit::qi;
			using namespace boost::phoenix;

			start =	(&day_month_year >> day_month_year [at_c<0>(_val) = false, at_c<1>(_val) = boost::spirit::_1])
					| (&month_day_year >> month_day_year [at_c<0>(_val) = false, at_c<1>(_val) = boost::spirit::_1])
					| (&year_month_day >> year_month_day [at_c<0>(_val) = false, at_c<1>(_val) = boost::spirit::_1]);

			short_month_str.add
				("Jan", 0)
				("Feb", 1)
				("Mar", 2)
				("Apr", 3)
				("May", 4)
				("Jun", 5)
				("Jul", 6)
				("Aug", 7)
				("Sep", 8)
				("Oct", 9)
				("Nov", 10)
				("Dec", 11)
				;

			long_month_str.add
				("January", 0)
				("February", 1)
				("March", 2)
				("April", 3)
				("May", 4)
				("June", 5)
				("July", 6)
				("August", 7)
				("September", 8)
				("October", 9)
				("November", 10)
				("December", 11)
				;

			day_month_year = int_ [at_c<2>(_val) = boost::spirit::_1, _pass = boost::spirit::_1 < 32] >> (long_month_str | short_month_str) [at_c<1>(_val) = boost::spirit::_1] >> int_ [at_c<0>(_val) = boost::spirit::_1 - 1900];
			year_month_day = int_ [at_c<0>(_val) = boost::spirit::_1 - 1900, _pass = boost::spirit::_1 > 1900] >> (long_month_str | short_month_str) [at_c<1>(_val) = boost::spirit::_1] >> int_ [at_c<2>(_val) = boost::spirit::_1, _pass = boost::spirit::_1 < 32];
			month_day_year = (long_month_str | short_month_str ) [at_c<1>(_val) = boost::spirit::_1] >>  int_ [at_c<2>(_val) = boost::spirit::_1] >> char_(',') >> int_ [at_c<0>(_val) = boost::spirit::_1 - 1900];

			// BOOST_SPIRIT_DEBUG_NODE(start);
		}

}
