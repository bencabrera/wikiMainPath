#include "countArticleLengthsHandler.h"

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/replace.hpp>

// boost.spirit stuff
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_repeat.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

#include <iomanip>


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

// from: http://stackoverflow.com/a/17708801/1903364
std::string CountArticleLengthHandler::url_encode(const std::string &value) 
{
	std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
		std::string::value_type c = (*i);

        // Keep alphanumeric and other accepted characters intact
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }

        // Any other characters are percent-encoded
        escaped << std::uppercase;
        escaped << '%' << std::setw(2) << int((unsigned char) c);
        escaped << std::nouppercase;
    }

    return escaped.str();
}

void CountArticleLengthHandler::clean_and_encode_title(std::string& title)
{
	boost::trim(title);
	boost::replace_all(title, " ", "_");
	title = url_encode(title);
}

void CountArticleLengthHandler::HandleArticle(const WikiXmlDumpXerces::WikiPageData& data)
{
	std::string title = data.MetaData.at("title");
	clean_and_encode_title(title);

	auto p = existing_results[0].end(); // map it to correct element 
	for (auto& map : existing_results) {
		auto tmp_it = map.find(title);
		if(tmp_it != map.end())
		{
			p = tmp_it;
			break;
		}
	}
	 
	if(p != existing_results[0].end())
	{
		std::string cleaned = preprocess(data.Content);

		auto props = compute_properties(cleaned);
		p->second.push_back(std::to_string(std::get<0>(props)));
		p->second.push_back(std::to_string(std::get<1>(props)));
		p->second.push_back(std::to_string(std::get<2>(props)));
	}
}
