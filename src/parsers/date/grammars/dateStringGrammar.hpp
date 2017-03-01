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

namespace WikiMainPath {

	template<typename Iterator, typename Skipper>
		struct DateStringGrammar : boost::spirit::qi::grammar<Iterator, Date(), Skipper> {
			DateStringGrammar();

			boost::spirit::qi::rule<Iterator, std::tm(), Skipper> template_year_month_day;
			boost::spirit::qi::rule<Iterator, Date(), Skipper> type1_template;
			boost::spirit::qi::rule<Iterator, Date(), Skipper> type2_template;
			boost::spirit::qi::rule<Iterator, Skipper> type2_parameter;

			boost::spirit::qi::rule<Iterator, Date(), Skipper> fuzzy_template_dates;

			boost::spirit::qi::symbols<char> type1_string;
			boost::spirit::qi::symbols<char> fuzzy_template_names;

			boost::spirit::qi::rule<Iterator, Date(), Skipper> start;

			boost::spirit::qi::rule<Iterator, std::tm(), Skipper> day_month_year;
			boost::spirit::qi::rule<Iterator, std::tm(), Skipper> month_day_year;
			boost::spirit::qi::rule<Iterator, std::tm(), Skipper> year_month_day;

			boost::spirit::qi::symbols<char,int> short_month_str;
			boost::spirit::qi::symbols<char,int> long_month_str;
		};

}
