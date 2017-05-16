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
		struct DateStringGrammar : boost::spirit::qi::grammar<Iterator, std::pair<bool,Date>(), Skipper> {
			DateStringGrammar();

			boost::spirit::qi::rule<Iterator, std::pair<bool,Date>(), Skipper> start;

			boost::spirit::qi::rule<Iterator, Date(), Skipper> day_range_dmy_date;
			boost::spirit::qi::rule<Iterator, Date(), Skipper> month_range_dmy_date;
			boost::spirit::qi::rule<Iterator, Date(), Skipper> year_range_dmy_date;

			boost::spirit::qi::rule<Iterator, Date(), Skipper> day_range_mdy_date;
			boost::spirit::qi::rule<Iterator, Date(), Skipper> month_range_mdy_date;

			boost::spirit::qi::rule<Iterator, Date(), Skipper> month_year_date;
			boost::spirit::qi::rule<Iterator, Date(), Skipper> day_month_year_date;
			boost::spirit::qi::rule<Iterator, Date(), Skipper> month_day_year_date;
			boost::spirit::qi::rule<Iterator, Date(), Skipper> year_month_day_date;

			boost::spirit::qi::rule<Iterator, std::tm(), Skipper> month_year;
			boost::spirit::qi::rule<Iterator, std::tm(), Skipper> day_month_year;
			boost::spirit::qi::rule<Iterator, std::tm(), Skipper> month_day_year;
			boost::spirit::qi::rule<Iterator, std::tm(), Skipper> year_month_day;

			boost::spirit::qi::symbols<char,int> short_month_str;
			boost::spirit::qi::symbols<char,int> long_month_str;
			boost::spirit::qi::symbols<char> to_separator;
			boost::spirit::qi::symbols<char> numeric_abbrv;
		};

}
