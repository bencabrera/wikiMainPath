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
#include "dateStringGrammar.hpp"

namespace WikiMainPath {

	template<typename Iterator, typename Skipper>
		struct InfoboxGrammar : boost::spirit::qi::grammar<Iterator, std::vector<std::pair<std::string, Date>>(), Skipper> {
			InfoboxGrammar();

			boost::spirit::qi::rule<Iterator, std::vector<std::pair<std::string, Date>>(), Skipper> article;

			boost::spirit::qi::rule<Iterator, std::vector<std::pair<std::string, Date>>(), Skipper> infobox;

			boost::spirit::qi::rule<Iterator, std::string()> infobox_label;
			boost::spirit::qi::rule<Iterator, std::pair<std::string, Date>(), Skipper> infobox_line_with_date;

			DateStringGrammar<Iterator, Skipper> date_string;


			// boost::spirit::qi::symbols<char,int> short_month_str;
			// boost::spirit::qi::symbols<char,int> long_month_str;
		};

}
