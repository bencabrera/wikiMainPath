#include "stringDateExtraction.h"

#include <boost/algorithm/string/trim.hpp>

#include "grammars/dateStringGrammar.hpp"
#include "grammars/wikiDateTemplateGrammar.hpp"
#include "grammars/wikiFuzzyDateTemplateGrammar.hpp"
#include "grammars/infoboxKeyValueGrammar.hpp"
#include "../../core/adaptDate.h"

namespace WikiMainPath {


	bool extractDateFromString(const std::string& str, Date& date)
	{
		bool found = false;
		try{
			WikiMainPath::WikiDateTemplateGrammar<std::string::const_iterator, boost::spirit::qi::blank_type> wiki_date_template_grammar;
			std::pair<bool, Date> p;
			auto it = str.cbegin();
			boost::spirit::qi::phrase_parse(it, str.cend(), wiki_date_template_grammar, boost::spirit::qi::blank, p);
			if(p.first)
			{
				date = p.second;
				found = true;
			}
		}
		catch(boost::spirit::qi::expectation_failure<std::string::const_iterator> e)
		{
			found = false;
		}

		if(!found)
		{
			try{
				WikiMainPath::WikiFuzzyDateTemplateGrammar<std::string::const_iterator, boost::spirit::qi::blank_type> wiki_date_template_grammar;
				std::pair<bool, Date> p;
				auto it = str.cbegin();
				boost::spirit::qi::phrase_parse(it, str.cend(), wiki_date_template_grammar, boost::spirit::qi::blank, p);
				if(p.first)
				{
					date = p.second;
					found = true;
				}
			}
			catch(boost::spirit::qi::expectation_failure<std::string::const_iterator> e)
			{
				found = false;
			}
		}

		if(!found)
		{
			try{
				WikiMainPath::DateStringGrammar<std::string::const_iterator, boost::spirit::qi::blank_type> date_string_grammar;
				std::pair<bool, Date> p;
				auto it = str.cbegin();
				boost::spirit::qi::phrase_parse(it, str.cend(), date_string_grammar, boost::spirit::qi::blank, p);
				if(p.first)
				{
					date = p.second;
					found = true;
				}
			}
			catch(boost::spirit::qi::expectation_failure<std::string::const_iterator> e)
			{
				found = false;
			}
		}

		return found;
	}

}
