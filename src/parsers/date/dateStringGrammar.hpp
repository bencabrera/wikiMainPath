#pragma once

#define BOOST_SPIRIT_DEBUG

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

#include "date.h"

namespace WikiTalkNet {

	template<typename Iterator, typename Skipper>
		struct DateStringGrammar : boost::spirit::qi::grammar<Iterator, std::size_t, Skipper> {
			DateStringGrammar();

			boost::spirit::qi::rule<Iterator, std::tm(), Skipper> year_month_day;
			boost::spirit::qi::rule<Iterator, Date(), Skipper> age;

			boost::spirit::qi::rule<Iterator, Date(), Skipper> start;
		};

}
