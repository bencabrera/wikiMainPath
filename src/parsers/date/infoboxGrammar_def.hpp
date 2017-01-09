#pragma once

#define BOOST_SPIRIT_DEBUG

#include "infoboxGrammar.hpp"

namespace WikiMainPath {

	template<typename Iterator, typename Skipper>
		InfoboxGrammar<Iterator, Skipper>::InfoboxGrammar() : InfoboxGrammar::base_type(article, "InfoboxGrammar") {
			using namespace boost::spirit::qi;
			using namespace boost::phoenix;

			article = *(char_ - '{') >> ((&(lit("{{") >> no_case["infobox"]) > infobox [_val = boost::spirit::_1]) | ('{' >> article));

			infobox = no_skip[lit("{{") >> no_case["infobox"] >> *char_] >> eol 
						>> ((&infobox_line_with_date >> infobox_line_with_date [boost::phoenix::push_back(_val, boost::spirit::_1)])| *char_) % eol
						>> lit("}}");

			infobox_line_with_date = lit('|') >> no_skip[(*(char_ - '=') [at_c<0>(_val) += boost::spirit::_1])] >> '=' >> date_string [at_c<1>(_val) = boost::spirit::_1];




			// BOOST_SPIRIT_DEBUG_NODE(article);
			// BOOST_SPIRIT_DEBUG_NODE(infobox);
			// BOOST_SPIRIT_DEBUG_NODE(infobox_line_with_date);
			// // BOOST_SPIRIT_DEBUG_NODE(start);

			// start.name("CommentGrammar::Start");
		}

}
