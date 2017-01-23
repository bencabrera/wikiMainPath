#pragma once

#define BOOST_SPIRIT_DEBUG

#include "infoboxKeyValueGrammar.hpp"

#include <boost/phoenix/bind/bind_member_function.hpp>

namespace WikiMainPath {

	template<typename Iterator, typename Skipper>
		InfoboxKeyValueGrammar<Iterator, Skipper>::InfoboxKeyValueGrammar() : InfoboxKeyValueGrammar::base_type(article, "InfoboxKeyValueGrammar") {
			using namespace boost::spirit::qi;
			using namespace boost::phoenix;

			article = *(char_ - '{') >> ((&(lit("{{") >> no_case["infobox"]) > infobox [_val = boost::spirit::_1]) | ('{' >> article));

			start_infobox = lexeme[lit("{{") >> no_case["infobox"] >> *(char_ - eol) >> eol];
			infobox = start_infobox >> 
						// ((&infobox_line >> infobox_line [push_back(_val, boost::spirit::_1)])
						 // | *(!lit("}}") >> (char_ - eol))) % (eol >> !lit("}}"))
						infobox_line % (eol >> !lit("}}"))
						>> *eol >> lit("}}");

			infobox_line_key = +(char_ - '=' - eol) [_val += boost::spirit::_1];
			infobox_line_value = *(char_ - eol - '{' - '}') [_val += boost::spirit::_1] >> -(
					(nested_meta_command [_val += boost::spirit::_1] >> infobox_line_value [_val += boost::spirit::_1])
					| (!(*space >> lit("}}")) >> char_("{}") [_val += boost::spirit::_1] >> infobox_line_value [_val += boost::spirit::_1]) 
				);

			infobox_line = lit('|') >> infobox_line_key [at_c<0>(_val) = boost::spirit::_1] >> '=' >> infobox_line_value [at_c<1>(_val) = boost::spirit::_1];

			nested_meta_command = &lit("{{") >> string("{{") [_val += boost::spirit::_1] >> text_in_command [_val += boost::spirit::_1] >> string("}}") [_val += boost::spirit::_1];
			text_in_command = *(char_ - '}' - '{') [_val += boost::spirit::_1] >> (
					(&lit("{{") >> nested_meta_command [_val += boost::spirit::_1] >> text_in_command [_val += boost::spirit::_1] ) 
					| &lit("}}") 
					| (char_("{}") [_val += boost::spirit::_1] >> text_in_command[_val += boost::spirit::_1])
				);

			// BOOST_SPIRIT_DEBUG_NODE(article);
			// BOOST_SPIRIT_DEBUG_NODE(infobox);
			// BOOST_SPIRIT_DEBUG_NODE(infobox_line);
			// BOOST_SPIRIT_DEBUG_NODE(infobox_line_key);
			// BOOST_SPIRIT_DEBUG_NODE(infobox_line_value);
			// BOOST_SPIRIT_DEBUG_NODE(nested_meta_command);
			// BOOST_SPIRIT_DEBUG_NODE(text_in_command);
			// BOOST_SPIRIT_DEBUG_NODE(start_infobox);
		}

}
