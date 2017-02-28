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

#include "../../core/date.h"
#include "../../core/adaptDate.h"

namespace WikiMainPath {

	template<typename Iterator, typename Skipper>
		struct WikiDateTemplateGrammar : boost::spirit::qi::grammar<Iterator, Date(), Skipper> {
			WikiDateTemplateGrammar();

			boost::spirit::qi::rule<Iterator, std::tm(), Skipper> template_year_month_day;
			boost::spirit::qi::rule<Iterator, Date(), Skipper> old_new_template;
			boost::spirit::qi::rule<Iterator, Date(), Skipper> new_old_template;
			boost::spirit::qi::rule<Iterator, Date(), Skipper> type2_template;
			boost::spirit::qi::rule<Iterator, Skipper> template_non_int_parameter;

			boost::spirit::qi::symbols<char> old_new_labels;
			boost::spirit::qi::symbols<char> new_old_labels;

			boost::spirit::qi::rule<Iterator, Date(), Skipper> start;
		};

}
