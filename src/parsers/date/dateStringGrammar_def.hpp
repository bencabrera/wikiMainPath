#pragma once

#define BOOST_SPIRIT_DEBUG

#include "dateStringGrammar.hpp"

namespace WikiTalkNet {

	template<typename Iterator, typename Skipper>
		DateStringGrammar<Iterator, Skipper>::DateStringGrammar() : DateStringGrammar::base_type(start, "DateStringGrammar") {
			using namespace boost::spirit::qi;
			using namespace boost::phoenix;

			// text = no_skip[*(char_ - '[' - '(')]  >> -(!(signature >> comment_ending) >> (char_('[')  | char_('(') )  >> text );
			// indentation = eps [_val = 0] >> *(char_(':') [_val++]);
			// start =  *eol >> indentation [at_c<3>(_val) = boost::spirit::qi::_1] >> text [at_c<0>(_val) = boost::spirit::_1] >> signature [at_c<1>(_val) = at_c<0>(boost::spirit::_1), at_c<2>(_val) = at_c<1>(boost::spirit::_1)];

			// // BOOST_SPIRIT_DEBUG_NODE(text);
			// // BOOST_SPIRIT_DEBUG_NODE(start);

			// start.name("CommentGrammar::Start");
			// text.name("CommentGrammar::Text");
		}

}
