#pragma once

#define BOOST_SPIRIT_DEBUG

#include "infoboxGrammar.hpp"

#include <boost/phoenix/bind/bind_member_function.hpp>

namespace WikiMainPath {

	template<typename Iterator, typename Skipper>
		InfoboxGrammar<Iterator, Skipper>::InfoboxGrammar() : InfoboxGrammar::base_type(article, "InfoboxGrammar") {
			using namespace boost::spirit::qi;
			using namespace boost::phoenix;

			article = *(char_ - '{') >> ((&(lit("{{") >> no_case["infobox"]) > infobox [_val = boost::spirit::_1]) | ('{' >> article));

			start_infobox = lexeme[lit("{{") >> no_case["infobox"] >> *(char_ - eol) >> eol];
			infobox = start_infobox >> 
						((&infobox_line_with_date >> infobox_line_with_date [
							  std::cout << "WORKED" << std::endl, 
							 push_back(_val, boost::spirit::_1)
							 // push_back(boost::phoenix::ref(help_dat), boost::spirit::_1)
							 // std::cout << "hell" << boost::phoenix::bind(&std::vector<std::pair<std::string, Date>>::size, boost::phoenix::arg_names::arg1)(_val) << "hull" << std::endl
						])| *(char_ - eol)) % (eol >> !lit("}}"))
						>> -eol >> lit("}}");

			infobox_label = *(char_ - '=') [_val += boost::spirit::_1] >> '=';
			infobox_line_with_date = lit('|') >> infobox_label [std::cout << "hallo" << std::endl, at_c<0>(_val) = boost::spirit::_1] >> date_string [at_c<1>(_val) = boost::spirit::_1];

			BOOST_SPIRIT_DEBUG_NODE(article);
			BOOST_SPIRIT_DEBUG_NODE(infobox);
			BOOST_SPIRIT_DEBUG_NODE(infobox_line_with_date);
			BOOST_SPIRIT_DEBUG_NODE(infobox_label);
			BOOST_SPIRIT_DEBUG_NODE(start_infobox);
		}

}
