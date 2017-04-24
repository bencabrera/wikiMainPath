#pragma once

#define BOOST_SPIRIT_DEBUG

#include "plainYearGrammar.hpp"

namespace WikiMainPath {

	template<typename Iterator, typename Skipper>
		PlainYearGrammar<Iterator, Skipper>::PlainYearGrammar() : PlainYearGrammar::base_type(start, "PlainYearGrammar") {
			using namespace boost::spirit::qi;
			using namespace boost::phoenix;

			start =	number_year_date [at_c<0>(_val) = true, at_c<1>(_val) = boost::spirit::_1];


			number_year_date = (&number_year_ad >> number_year_ad [at_c<0>(_val) = false, at_c<1>(_val) = boost::spirit::_1])
								| (&number_year_bc >> number_year_bc [at_c<0>(_val) = false, at_c<1>(_val) = boost::spirit::_1])
								| (&number_year >> number_year [at_c<0>(_val) = false, at_c<1>(_val) = boost::spirit::_1]);

			number_year = -no_case[circa_strs] >> int_ [_pass = (boost::spirit::_1 < 2050) && (boost::spirit::_1 > 0), at_c<0>(_val) = boost::spirit::_1 - 1900, at_c<1>(_val) = 6, at_c<2>(_val) = 15, at_c<3>(_val) = 2];
			number_year_ad = -no_case[circa_strs] >> lit("AD") >> int_ [_pass = boost::spirit::_1 > 0, at_c<0>(_val) = (-1*boost::spirit::_1) - 1900, at_c<1>(_val) = 6, at_c<2>(_val) = 15, at_c<3>(_val) = 2];
			number_year_bc = -no_case[circa_strs] >> int_ [_pass = boost::spirit::_1 > 0, at_c<0>(_val) = (-1*boost::spirit::_1) - 1900, at_c<1>(_val) = 6, at_c<2>(_val) = 15, at_c<3>(_val) = 2] >> lit("BC");

			circa_strs = "c.", "[[circa|c.]]", "ca.", "circa", "{{circa}}", "{{ circa }}", "{{circa|", "{{c.|","{{ca.|", "by", "<br>";
			// BOOST_SPIRIT_DEBUG_NODE(start);
		}

}
