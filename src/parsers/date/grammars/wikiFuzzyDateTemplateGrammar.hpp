#pragma once

// #define BOOST_SPIRIT_DEBUG

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_repeat.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/spirit/include/qi_symbols.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>

#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>

#include "../../../core/date.h"
#include "../../../core/adaptDate.h"
#include "dateStringGrammar.hpp"

namespace WikiMainPath {

	template<typename Iterator, typename Skipper>
		struct WikiFuzzyDateTemplateGrammar : boost::spirit::qi::grammar<Iterator, std::pair<bool,Date>(), Skipper> {
			WikiFuzzyDateTemplateGrammar();

			boost::spirit::qi::rule<Iterator, Date(), Skipper> fuzzy_template_dates;
			boost::spirit::qi::symbols<char> fuzzy_template_names;

			boost::spirit::qi::rule<Iterator, std::pair<bool,Date>(), Skipper> start;
	
			DateStringGrammar<Iterator, Skipper> date_string_grammar;
		};

}
