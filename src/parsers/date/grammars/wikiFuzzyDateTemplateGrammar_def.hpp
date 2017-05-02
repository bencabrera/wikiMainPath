#pragma once

#define BOOST_SPIRIT_DEBUG

#include "wikiFuzzyDateTemplateGrammar.hpp"

namespace WikiMainPath {

	template<typename Iterator, typename Skipper>
		WikiFuzzyDateTemplateGrammar<Iterator, Skipper>::WikiFuzzyDateTemplateGrammar() : WikiFuzzyDateTemplateGrammar::base_type(start, "WikiFuzzyDateTemplateGrammar") {
			using namespace boost::spirit::qi;
			using namespace boost::phoenix;

			year_fuzzy_template_names = "birth-date", "death-date", "start-date", "end-date", "birth date", "death date", "start-date", "end-date", "film date", "birth date and age", "birth-date and age", "start date", "start-date", "start date and years ago", "start-date and years ago", "start date and age", "start-date and age", "start year and age","start-year and age",  "birth-year and age", "birth year and age", "death-year and age", "death year and age", "birth year", "birth-year", "death-year", "death year";
			single_fuzzy_template_names = "birth-date", "death-date", "start-date", "end-date", "birth date", "death date", "start date", "end-date", "film date";
			old_new_fuzzy_template_names = "birth-year and age", "birth year and age", "birth date and age", "birth-date and age", "start date and years ago", "start-date and years ago", "start date and age", "start-date and age";
			new_old_fuzzy_template_names = "death-year and age", "death year and age", "death date and age", "death-date and age", "death year and age", "death-year and age";


			single_fuzzy_template_dates = 
				lit("{{") >> no_case[single_fuzzy_template_names] >> '|' 
				>> *(&wiki_date_template_grammar.template_non_int_parameter >> wiki_date_template_grammar.template_non_int_parameter >> '|') 
				>> (date_string_grammar.day_month_year_date [_val = boost::spirit::_1] | date_string_grammar.month_day_year_date [_val = boost::spirit::_1])
				>> *(char_ - '}') 
				>> lit("}}");

			old_new_fuzzy_template_dates = 
				lit("{{") >> no_case[old_new_fuzzy_template_names] >> '|' 
				>> *(&wiki_date_template_grammar.template_non_int_parameter >> wiki_date_template_grammar.template_non_int_parameter >> '|') 
				>> (date_string_grammar.day_month_year [at_c<1>(_val) = boost::spirit::_1] | date_string_grammar.month_day_year [at_c<1>(_val) = boost::spirit::_1])
				>> '|' 
				>> (date_string_grammar.day_month_year [at_c<2>(_val) = boost::spirit::_1, at_c<0>(_val) = true] | date_string_grammar.month_day_year [at_c<2>(_val) = boost::spirit::_1, at_c<0>(_val) = true])
				>> *(char_ - '}') 
				>> lit("}}");

			new_old_fuzzy_template_dates = 
				lit("{{") >> no_case[new_old_fuzzy_template_names] >> '|' 
				>> *(&wiki_date_template_grammar.template_non_int_parameter >> wiki_date_template_grammar.template_non_int_parameter >> '|') 
				>> (date_string_grammar.day_month_year [at_c<2>(_val) = boost::spirit::_1] | date_string_grammar.month_day_year [at_c<2>(_val) = boost::spirit::_1])
				>> '|' 
				>> (date_string_grammar.day_month_year [at_c<1>(_val) = boost::spirit::_1, at_c<0>(_val) = true] | date_string_grammar.month_day_year [at_c<1>(_val) = boost::spirit::_1, at_c<0>(_val) = true])
				>> *(char_ - '}') 
				>> lit("}}");

			single_fuzzy_template_year = 
				lit("{{") >> no_case[year_fuzzy_template_names] >> '|' 
				>> *(&wiki_date_template_grammar.template_non_int_parameter >> wiki_date_template_grammar.template_non_int_parameter >> '|') 
				>> plain_year_grammar.without_eoi [_val = at_c<1>(boost::spirit::_1)]
				// >> -(wiki_date_template_grammar.template_non_int_parameter % '|') 
				>> lit("}}");

			old_new_fuzzy_template_year = 
				lit("{{") >> no_case[year_fuzzy_template_names] >> '|' 
				>> *(&wiki_date_template_grammar.template_non_int_parameter >> wiki_date_template_grammar.template_non_int_parameter >> '|') 
				>> plain_year_grammar.without_eoi [at_c<1>(_val) = at_c<1>(at_c<1>(boost::spirit::_1))]
				>> '|' 
				>> plain_year_grammar.without_eoi [at_c<2>(_val) = at_c<1>(at_c<1>(boost::spirit::_1))]
				// >> -(wiki_date_template_grammar.template_non_int_parameter % '|') 
				>> lit("}}");

			new_old_fuzzy_template_year = 
				lit("{{") >> no_case[year_fuzzy_template_names] >> '|' 
				>> *(&wiki_date_template_grammar.template_non_int_parameter >> wiki_date_template_grammar.template_non_int_parameter >> '|') 
				>> plain_year_grammar.without_eoi [at_c<2>(_val) = at_c<1>(at_c<1>(boost::spirit::_1))]
				>> '|' 
				>> plain_year_grammar.without_eoi [at_c<1>(_val) = at_c<1>(at_c<1>(boost::spirit::_1))]
				// >> -(wiki_date_template_grammar.template_non_int_parameter % '|') 
				>> lit("}}");

			start =	
				eps [at_c<0>(_val) = false]
				>> *(!lit("{{") >> char_)
				>> (
					(&old_new_fuzzy_template_dates >> old_new_fuzzy_template_dates [at_c<0>(_val) = true, at_c<1>(_val) = boost::spirit::_1])
					| (&new_old_fuzzy_template_dates >> new_old_fuzzy_template_dates [at_c<0>(_val) = true, at_c<1>(_val) = boost::spirit::_1])
					| (&single_fuzzy_template_dates >> single_fuzzy_template_dates [at_c<0>(_val) = true, at_c<1>(_val) = boost::spirit::_1])
					| (&old_new_fuzzy_template_year >> old_new_fuzzy_template_year [at_c<0>(_val) = true, at_c<1>(_val) = boost::spirit::_1])
					| (&new_old_fuzzy_template_year >> new_old_fuzzy_template_year [at_c<0>(_val) = true, at_c<1>(_val) = boost::spirit::_1])
					| (&single_fuzzy_template_year >> single_fuzzy_template_year [at_c<0>(_val) = true, at_c<1>(_val) = boost::spirit::_1])
				   );
		}

}
