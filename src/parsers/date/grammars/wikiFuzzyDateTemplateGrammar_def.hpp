#pragma once

#define BOOST_SPIRIT_DEBUG

#include "wikiFuzzyDateTemplateGrammar.hpp"

namespace WikiMainPath {

	template<typename Iterator, typename Skipper>
		WikiFuzzyDateTemplateGrammar<Iterator, Skipper>::WikiFuzzyDateTemplateGrammar() : WikiFuzzyDateTemplateGrammar::base_type(start, "WikiFuzzyDateTemplateGrammar") {
			using namespace boost::spirit::qi;
			using namespace boost::phoenix;

			// TODO: solve this with no_case
			fuzzy_template_names = "Birth-date", "Death-date", "Start-date", "End-date", "birth-date", "birth date", "death-date", "death date", "start-date", "end-date", "birth date and age", "death date and age", "Birth date and age", "Death date and age";

			fuzzy_template_dates = 
				lit("{{") >> fuzzy_template_names >> '|' 
				>> date_string_grammar [_val = at_c<1>(boost::spirit::_1)] 
				>> *(char_ - '}') 
				>> lit("}}");

			start =	
				eps [at_c<0>(_val) = false]
				>> *(!lit("{{") >> char_)
				>> (&fuzzy_template_dates >> fuzzy_template_dates [at_c<0>(_val) = true, at_c<1>(_val) = boost::spirit::_1]);

			// BOOST_SPIRIT_DEBUG_NODE(start);
		}

}
