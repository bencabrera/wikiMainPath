#pragma once

#define BOOST_SPIRIT_DEBUG

#include "plainYearGrammar.hpp"

namespace WikiMainPath {

	template<typename Iterator, typename Skipper>
		PlainYearGrammar<Iterator, Skipper>::PlainYearGrammar() : PlainYearGrammar::base_type(start, "PlainYearGrammar") {
			using namespace boost::spirit::qi;
			using namespace boost::phoenix;

			start =	(&number_year_date >> number_year_date [at_c<0>(_val) = true, at_c<1>(_val) = boost::spirit::_1]);


			number_year_date = number_year [at_c<0>(_val) = false, at_c<1>(_val) = boost::spirit::_1];

			number_year = int_ [_pass = (boost::spirit::_1 < 2050) && (boost::spirit::_1 > 0), at_c<0>(_val) = boost::spirit::_1 - 1900, at_c<1>(_val) = 6, at_c<2>(_val) = 15];

			// BOOST_SPIRIT_DEBUG_NODE(start);
		}

}
