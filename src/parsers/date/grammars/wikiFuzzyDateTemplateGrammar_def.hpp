#pragma once

#define BOOST_SPIRIT_DEBUG

#include "wikiFuzzyDateTemplateGrammar.hpp"

namespace WikiMainPath {

	template<typename Iterator, typename Skipper>
		WikiFuzzyDateTemplateGrammar<Iterator, Skipper>::WikiFuzzyDateTemplateGrammar() : WikiFuzzyDateTemplateGrammar::base_type(start, "WikiFuzzyDateTemplateGrammar") {
			using namespace boost::spirit::qi;
			using namespace boost::phoenix;

			fuzzy_template_names = "birth-date", "death-date", "start-date", "end-date", "birth date", "death date", "start-date", "end-date", "birth date and age", "death date and age", "birth date and age", "death date and age", "film date", "start date and years ago";


			// for when the template contains something like |df=m 
			template_non_int_parameter = *(char_ - '=' - eol - '|' - '}') >> '=' >> *(char_ - '|' - eol - '}');

			fuzzy_template_dates = 
				lit("{{") >> no_case[fuzzy_template_names] >> '|' 
				>> *(&template_non_int_parameter >> template_non_int_parameter >> '|') 
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
