#pragma once

#define BOOST_SPIRIT_DEBUG

#include "dateStringGrammar.hpp"

namespace WikiMainPath {

	template<typename Iterator, typename Skipper>
		DateStringGrammar<Iterator, Skipper>::DateStringGrammar() : DateStringGrammar::base_type(start, "DateStringGrammar") {
			using namespace boost::spirit::qi;
			using namespace boost::phoenix;

			// TODO: solve this with no_case
			type1_string = "Age", "age", "Age nts", "age nts", "Age for infant", "age for infant", "Birth date", "birth date", "Death date", "Start date", "End date", "death date", "start date", "end date", "start date and age";
			fuzzy_template_names = "Birth-date", "Death-date", "Start-date", "End-date", "birth-date", "death-date", "start-date", "end-date";

			template_year_month_day = int_ [at_c<0>(_val) = boost::spirit::_1 - 1900] >> '|' >> int_ [at_c<1>(_val) = boost::spirit::_1 - 1] >> '|' >> int_ [at_c<2>(_val) = boost::spirit::_1];

			fuzzy_template_dates = eps [at_c<0>(_val) = false] >> lit("{{") >> fuzzy_template_names >> '|' >> (
									(
										 &day_month_year >> day_month_year [at_c<1>(_val) = boost::spirit::_1])
										| (&month_day_year >> month_day_year [at_c<1>(_val) = boost::spirit::_1])
										| (&year_month_day >> year_month_day [at_c<1>(_val) = boost::spirit::_1])
									) >> *(char_ - '}') >> lit("}}");

			type1_template = eps [at_c<0>(_val) = false] >> lit("{{") >> type1_string >> '|' >> template_year_month_day [at_c<1>(_val) = boost::spirit::_1] >> -('|' >> (template_year_month_day [at_c<0>(_val) = true, at_c<2>(_val) = boost::spirit::_1] | *(char_ - '}'))) >> lit("}}");

			type2_parameter = lit("|") >> *(char_ - '=' - eol) >> '=' >> *(char_ - '|' - eol);
			type2_template = eps [at_c<0>(_val) = false] >> lit("{{") >> type1_string >> 
				*(&type2_parameter >> type2_parameter) >> 
				'|' >> template_year_month_day [at_c<1>(_val) = boost::spirit::_1] >> -('|' >> (template_year_month_day [at_c<0>(_val) = true, at_c<2>(_val) = boost::spirit::_1] | *(char_ - '}' - eol))) >> lit("}}");

			start = (&type1_template >> type1_template [_val = boost::spirit::_1])
					| (&type2_template >> type2_template [_val = boost::spirit::_1])
					| (&day_month_year >> day_month_year [at_c<0>(_val) = false, at_c<1>(_val) = boost::spirit::_1])
					| (&month_day_year >> month_day_year [at_c<0>(_val) = false, at_c<1>(_val) = boost::spirit::_1])
					| (&year_month_day >> year_month_day [at_c<0>(_val) = false, at_c<1>(_val) = boost::spirit::_1])
					| (&fuzzy_template_dates >> fuzzy_template_dates [_val = boost::spirit::_1]);

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

			// text = no_skip[*(char_ - '[' - '(')]  >> -(!(signature >> comment_ending) >> (char_('[')  | char_('(') )  >> text );
			// indentation = eps [_val = 0] >> *(char_(':') [_val++]);
			// start =  *eol >> indentation [at_c<3>(_val) = boost::spirit::qi::_1] >> text [at_c<0>(_val) = boost::spirit::_1] >> signature [at_c<1>(_val) = at_c<0>(boost::spirit::_1), at_c<2>(_val) = at_c<1>(boost::spirit::_1)];

			BOOST_SPIRIT_DEBUG_NODE(type2_parameter);
			BOOST_SPIRIT_DEBUG_NODE(type2_template);
			// BOOST_SPIRIT_DEBUG_NODE(age);
			// BOOST_SPIRIT_DEBUG_NODE(type1_string);
			BOOST_SPIRIT_DEBUG_NODE(start);
			// BOOST_SPIRIT_DEBUG_NODE(start);

			// start.name("CommentGrammar::Start");
		}

}
