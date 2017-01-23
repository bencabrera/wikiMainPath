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
		struct InfoboxKeyValueGrammar : boost::spirit::qi::grammar<Iterator, std::vector<std::pair<std::string, std::string>>(), Skipper> {
			InfoboxKeyValueGrammar();

			boost::spirit::qi::rule<Iterator, std::vector<std::pair<std::string, std::string>>(), Skipper> article;
			boost::spirit::qi::rule<Iterator, std::vector<std::pair<std::string, std::string>>(), Skipper> infobox;
			boost::spirit::qi::rule<Iterator, Skipper> start_infobox;
			boost::spirit::qi::rule<Iterator, std::pair<std::string, std::string>(), Skipper> infobox_line;
			boost::spirit::qi::rule<Iterator, std::string()> infobox_line_key;
			boost::spirit::qi::rule<Iterator, std::string()> infobox_line_value;
			boost::spirit::qi::rule<Iterator, std::string()> nested_meta_command;
			boost::spirit::qi::rule<Iterator, std::string()> text_in_command;
		};

}
