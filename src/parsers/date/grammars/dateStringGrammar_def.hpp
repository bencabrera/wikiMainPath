#pragma once

#define BOOST_SPIRIT_DEBUG

#include "dateStringGrammar.hpp"

namespace WikiMainPath {

	template<typename Iterator, typename Skipper>
		DateStringGrammar<Iterator, Skipper>::DateStringGrammar() : DateStringGrammar::base_type(start, "DateStringGrammar") {
			using namespace boost::spirit::qi;
			using namespace boost::phoenix;

			start =	(&day_range_dmy_date >> day_range_dmy_date [at_c<0>(_val) = true, at_c<1>(_val) = boost::spirit::_1])
					| (&month_range_dmy_date >> month_range_dmy_date [at_c<0>(_val) = true, at_c<1>(_val) = boost::spirit::_1])
					| (&year_range_dmy_date >> year_range_dmy_date [at_c<0>(_val) = true, at_c<1>(_val) = boost::spirit::_1])
					| (&day_month_year_date >> day_month_year_date [at_c<0>(_val) = true, at_c<1>(_val) = boost::spirit::_1])
					| (&month_day_year_date >> month_day_year_date [at_c<0>(_val) = true, at_c<1>(_val) = boost::spirit::_1])
					| (&year_month_day_date >> year_month_day_date [at_c<0>(_val) = true, at_c<1>(_val) = boost::spirit::_1])
					| (&month_year_date >> month_year_date [at_c<0>(_val) = true, at_c<1>(_val) = boost::spirit::_1]);

			short_month_str.add
				("jan", 0)
				("feb", 1)
				("mar", 2)
				("apr", 3)
				("may", 4)
				("jun", 5)
				("jul", 6)
				("aug", 7)
				("sep", 8)
				("oct", 9)
				("nov", 10)
				("dec", 11)
				;

			long_month_str.add
				("january", 0)
				("february", 1)
				("march", 2)
				("april", 3)
				("may", 4)
				("june", 5)
				("july", 6)
				("august", 7)
				("september", 8)
				("october", 9)
				("november", 10)
				("december", 11)
				;

			// to_separator = "-", "–", "to", "up to", "−";
			to_separator = 
				"\u2010", // "‐" HYPHEN
				"\u2011", // "‑" NON-BREAKING HYPHEN
				"\u2012", // "‒" FIGURE DASH
				"\u2013", // "–" EN DASH
				"\u2014", // "—" EM DASH
				"\u2015", // "―" HORIZONTAL BAR

				"\u002D", // "-" HYPHEN-MINUS
				"\u2212", // "−" MINUS

				"\uFE58", // "﹘" SMALL EM DASH
				"\uFE63", // "﹣" SMALL HYPHEN-MINUS
				"\uFF0D", // "－" FULLWIDTH HYPHEN-MINUS

				"to", 
				"up to"
			;

			numeric_abbrv = "th", "nd", "st";

			month_year_date = month_year [at_c<0>(_val) = false, at_c<1>(_val) = boost::spirit::_1];

			day_range_dmy_date = 
				int_ [at_c<2>(at_c<1>(_val)) = boost::spirit::_1, _pass = boost::spirit::_1 < 32] >> to_separator
				>> int_ [at_c<2>(at_c<2>(_val)) = boost::spirit::_1, _pass = boost::spirit::_1 < 32]
				>> (no_case[long_month_str] | no_case[short_month_str]) [at_c<1>(at_c<1>(_val)) = boost::spirit::_1, at_c<1>(at_c<2>(_val)) = boost::spirit::_1] 
				>> int_ [at_c<0>(_val) = true, at_c<0>(at_c<1>(_val)) = boost::spirit::_1 - 1900, at_c<0>(at_c<2>(_val)) = boost::spirit::_1 - 1900];

			month_range_dmy_date = 
				int_ [at_c<2>(at_c<1>(_val)) = boost::spirit::_1, _pass = boost::spirit::_1 < 32]  
				>> (no_case[long_month_str] | no_case[short_month_str]) [at_c<1>(at_c<1>(_val)) = boost::spirit::_1] 
				>> to_separator 
				>> int_ [at_c<2>(at_c<2>(_val)) = boost::spirit::_1, _pass = boost::spirit::_1 < 32]
				>> (no_case[long_month_str] | no_case[short_month_str]) [at_c<1>(at_c<2>(_val)) = boost::spirit::_1] 
				>> int_ [at_c<0>(_val) = true, at_c<0>(at_c<1>(_val)) = boost::spirit::_1 - 1900, at_c<0>(at_c<2>(_val)) = boost::spirit::_1 - 1900];

			year_range_dmy_date = 
				int_ [at_c<2>(at_c<1>(_val)) = boost::spirit::_1, _pass = boost::spirit::_1 < 32]  
				>> (no_case[long_month_str] | no_case[short_month_str]) [at_c<1>(at_c<1>(_val)) = boost::spirit::_1] 
				>> int_ [at_c<0>(at_c<1>(_val)) = boost::spirit::_1 - 1900]
				>> to_separator 
				>> int_ [at_c<2>(at_c<2>(_val)) = boost::spirit::_1, _pass = boost::spirit::_1 < 32]
				>> (no_case[long_month_str] | no_case[short_month_str]) [at_c<1>(at_c<2>(_val)) = boost::spirit::_1] 
				>> int_ [at_c<0>(_val) = true, at_c<0>(at_c<2>(_val)) = boost::spirit::_1 - 1900];
				
				// int_ [at_c<0>(_val) = true, at_c<0>(at_c<1>(_val)) = boost::spirit::_1];
			day_month_year_date = day_month_year [at_c<0>(_val) = false, at_c<1>(_val) = boost::spirit::_1];
			month_day_year_date = month_day_year [at_c<0>(_val) = false, at_c<1>(_val) = boost::spirit::_1];
			year_month_day_date = year_month_day [at_c<0>(_val) = false, at_c<1>(_val) = boost::spirit::_1];

			month_year = 
				(no_case[long_month_str] | no_case[short_month_str]) [at_c<1>(_val) = boost::spirit::_1] >> -lit(',') 
				>> int_ [at_c<0>(_val) = boost::spirit::_1 - 1900, at_c<2>(_val) = 15];
			day_month_year = 
				int_ [at_c<2>(_val) = boost::spirit::_1, _pass = boost::spirit::_1 < 32] 
				>> -(lit('-') | (lit('/') >> int_)) >> -lit(',') 
				>> (no_case[long_month_str] | no_case[short_month_str]) [at_c<1>(_val) = boost::spirit::_1] >> -lit(',')
				>> int_ [at_c<0>(_val) = boost::spirit::_1 - 1900];
			year_month_day = 
				int_ [at_c<0>(_val) = boost::spirit::_1 - 1900, _pass = boost::spirit::_1 > 1900] >> -lit(',') 
				>> (no_case[long_month_str] | no_case[short_month_str]) [at_c<1>(_val) = boost::spirit::_1] >> -lit(',') 
				>> int_ [at_c<2>(_val) = boost::spirit::_1, _pass = boost::spirit::_1 < 32];
			month_day_year = 
				(no_case[long_month_str] | no_case[short_month_str] ) [at_c<1>(_val) = boost::spirit::_1] >> -lit(',') 
				>> int_ [at_c<2>(_val) = boost::spirit::_1] >> -numeric_abbrv >> -lit(',') 
				>> int_ [at_c<0>(_val) = boost::spirit::_1 - 1900];

			// BOOST_SPIRIT_DEBUG_NODE(start);
		}

}
