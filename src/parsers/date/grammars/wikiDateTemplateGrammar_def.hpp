#pragma once

#define BOOST_SPIRIT_DEBUG

#include "wikiDateTemplateGrammar.hpp"

namespace WikiMainPath {

	template<typename Iterator, typename Skipper>
		WikiDateTemplateGrammar<Iterator, Skipper>::WikiDateTemplateGrammar() : WikiDateTemplateGrammar::base_type(start, "WikiDateTemplateGrammar") {
			using namespace boost::spirit::qi;
			using namespace boost::phoenix;

			// type1_string = "Age", "age", "Age nts", "age nts", "Age for infant", "age for infant", "Birth date", "birth date", "Death date", "Start date", "End date", "death date", "start date", "end date", "start date and age", "birth date and age", "death date and age", "Birth date and age", "Death date and age";

			// templates of the format [older_date] | [newer_date]
			old_new_labels = "age","age nts", "birth date", "birth-date", "birthdate", "death date", "start date", "end date", "start date and age", "birth date and age", "film date", "start date and years ago", "start date and age";

			// templates of the format [newer_date] | [older_date]
			new_old_labels = "death date and age", "death date", "death-date", "deathdate";

			// for when the template contains something like |df=m 
			template_non_int_parameter = *(char_ - '=' - eol - '|' - '}') >> '=' >> *(char_ - '|' - eol - '}');

			template_year_month_day = 
				int_ [at_c<0>(_val) = boost::spirit::_1 - 1900] >> '|' 
				>> int_ [at_c<1>(_val) = boost::spirit::_1 - 1] >> '|' 
				>> int_ [at_c<2>(_val) = boost::spirit::_1];


			old_new_template = 
				eps [at_c<0>(_val) = false] 
				>> "{{" >> no_case[old_new_labels] >> '|' 
				>> *(&template_non_int_parameter >> template_non_int_parameter >> '|') 
				>> template_year_month_day [at_c<1>(_val) = boost::spirit::_1] 
				>> -('|' >> (
								template_year_month_day [at_c<0>(_val) = true, at_c<2>(_val) = boost::spirit::_1] 
								| (template_non_int_parameter % lit('|'))
								| *(char_ - '}')
							)
					) 
				>> lit("}}");

			new_old_template = 
				eps [at_c<0>(_val) = false] 
				>> "{{" >> no_case[new_old_labels] >> '|' 
				>> *(&template_non_int_parameter >> template_non_int_parameter >> '|') 
				>> template_year_month_day [at_c<2>(_val) = boost::spirit::_1] 
				>> -('|' >> template_year_month_day [at_c<0>(_val) = true, at_c<1>(_val) = boost::spirit::_1]) 
				>> -(template_non_int_parameter % lit('|'))
				>> *(char_ - '}')
				>> "}}";

			start = 
				eps [at_c<0>(_val) = false]
				>> *(!lit("{{") >> char_)
				>> (
					(&old_new_template >> old_new_template [at_c<0>(_val) = true, at_c<1>(_val) = boost::spirit::_1])
					| (&new_old_template >> new_old_template [at_c<0>(_val) = true, at_c<1>(_val) = boost::spirit::_1])
				);


			// BOOST_SPIRIT_DEBUG_NODE(template_year_month_day);
		}

}
