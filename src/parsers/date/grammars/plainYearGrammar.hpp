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
		struct PlainYearGrammar : boost::spirit::qi::grammar<Iterator, std::pair<bool,Date>(), Skipper> {
			PlainYearGrammar();

			boost::spirit::qi::rule<Iterator, std::pair<bool,Date>(), Skipper> start;
			boost::spirit::qi::rule<Iterator, std::pair<bool,Date>(), Skipper> without_eoi;

			boost::spirit::qi::rule<Iterator, Date(), Skipper> number_year_date;

			boost::spirit::qi::rule<Iterator, std::tm(), Skipper> number_year;
			boost::spirit::qi::rule<Iterator, std::tm(), Skipper> number_year_ad;
			boost::spirit::qi::rule<Iterator, std::tm(), Skipper> number_year_bc;

			boost::spirit::qi::symbols<char> circa_strs;
		};

}
