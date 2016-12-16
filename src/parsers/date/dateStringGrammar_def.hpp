#pragma once

#define BOOST_SPIRIT_DEBUG

#include "dateStringGrammar.hpp"

namespace WikiTalkNet {

	template<typename Iterator, typename Skipper>
		DateStringGrammar<Iterator, Skipper>::DateStringGrammar() : DateStringGrammar::base_type(start, "DateStringGrammar") {
			using namespace boost::spirit::qi;
			using namespace boost::phoenix;

			year_month_day = int_ [at_c<0>(_val) = boost::spirit::_1] >> '|' >> int_ [at_c<1>(_val) = boost::spirit::_1] >> '|' >> int_ [at_c<2>(_val) = boost::spirit::_1];

			age = eps [at_c<0>(_val) = false] >> lit("{{") >> no_case[lit("Age")] >> year_month_day [at_c<1>(_val) = boost::spirit::_1] >> -('|' >> year_month_day [at_c<0>(_val) = true, at_c<2>(_val) = boost::spirit::_1]) >> lit("}}");

			start = &age >> age [_val = boost::spirit::_1];

			// text = no_skip[*(char_ - '[' - '(')]  >> -(!(signature >> comment_ending) >> (char_('[')  | char_('(') )  >> text );
			// indentation = eps [_val = 0] >> *(char_(':') [_val++]);
			// start =  *eol >> indentation [at_c<3>(_val) = boost::spirit::qi::_1] >> text [at_c<0>(_val) = boost::spirit::_1] >> signature [at_c<1>(_val) = at_c<0>(boost::spirit::_1), at_c<2>(_val) = at_c<1>(boost::spirit::_1)];

			// // BOOST_SPIRIT_DEBUG_NODE(text);
			// // BOOST_SPIRIT_DEBUG_NODE(start);

			// start.name("CommentGrammar::Start");
			// text.name("CommentGrammar::Text");
		}

}
