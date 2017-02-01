#include "countArticleLengthsHandler.h"

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/replace.hpp>

#include "../date/dateExtraction.h"

// boost.spirit stuff
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_repeat.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>


CountArticleLengthHandler::CountArticleLengthHandler(std::vector<std::map<std::string, std::vector<std::string>>>& exi)
	:existing_results(exi)
{}

std::string CountArticleLengthHandler::preprocess(const std::string& input)
{
	using namespace boost::spirit::qi;

	// preprocessing
	rule<std::string::const_iterator, std::string()> skip_meta_commands;
	rule<std::string::const_iterator> meta_command;
	rule<std::string::const_iterator> text_in_command;
	skip_meta_commands = *(char_ - '{') [_val += boost::spirit::_1] >> -((&lit("{{") >> meta_command | lit('{')) >> skip_meta_commands [_val += boost::spirit::_1]);
	meta_command = lit("{{") >> text_in_command >> lit("}}");
	text_in_command = *(char_ - '}' - '{') >> ((&lit("{{") >> meta_command >> text_in_command) | &lit("}}") | (char_("{}") >> text_in_command));
		
	std::string cleaned_content;
	auto it = input.cbegin();
	parse(it, input.cend(), skip_meta_commands, cleaned_content);

	return cleaned_content;
}


CountArticleLengthHandler::ArticleProperties CountArticleLengthHandler::compute_properties(const std::string& input)
{
	std::size_t num_words = 0, num_sentences = 0;
	std::string dummy;
	std::istringstream ss(input);
	while(std::getline(ss, dummy, ' '))
		num_words++;

	std::istringstream ss2(input);
	while(std::getline(ss2, dummy, '.'))
		num_sentences++;

	ArticleProperties props;
	std::get<0>(props) = input.size();
	std::get<1>(props) = num_words;
	std::get<2>(props) = num_sentences;

	return props;
}

void CountArticleLengthHandler::clean_and_encode_title(std::string& title)
{
	boost::trim(title);
	boost::replace_all(title, "\"", "\\\"");
}

void CountArticleLengthHandler::HandleArticle(const ArticleData& data)
{
	std::string title = data.MetaData.at("title");
	clean_and_encode_title(title);

	std::cout << "EXISTING: " << existing_results.size() << std::endl;
	auto it = existing_results[0].begin(); // map it to correct element 
	for (auto& map : existing_results) {
		auto tmp_it = map.find(title);
		if(tmp_it != map.end())
		{
			it = tmp_it;
			break;
		}
	}

	// if(it != existing_results[0].end())
	// {
		// std::string cleaned = preprocess(data.Content);

		// // auto props = compute_properties(cleaned);
		// // it->second.push_back(std::to_string(std::get<0>(props)));
		// // it->second.push_back(std::to_string(std::get<1>(props)));
		// // it->second.push_back(std::to_string(std::get<2>(props)));
	// }
}
