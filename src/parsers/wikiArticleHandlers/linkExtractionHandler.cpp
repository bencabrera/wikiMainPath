#include "linkExtractionHandler.h"

// boost spirit 
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_repeat.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

LinkExtractionHandler::LinkExtractionHandler(
	const std::vector<std::string>& arts, 
	const std::map<std::string, std::string>& redirs,
	std::vector<boost::container::flat_set<std::size_t>>& adjList,
	VectorMutex<1000>& vecMut
)
:_articles(arts),
_redirects(redirs),
_adjList(adjList),
_vecMutex(vecMut)
{
	if(_adjList.size() != _articles.size())
		throw std::logic_error("Dimensions are not correct.");
}

void LinkExtractionHandler::HandleArticle(const WikiXmlDumpXerces::WikiPageData& data)
{
	using namespace boost::spirit;

	std::string title = data.MetaData.at("title");
	boost::trim(title);

	std::size_t source, target;
	auto source_it = std::find(_articles.begin(), _articles.end(), title);
	if(source_it != _articles.end())
	{
		source = source_it - _articles.begin();
	}
	else
		return;

	qi::rule<std::string::const_iterator, std::vector<std::string>()> links_rule;
	qi::rule<std::string::const_iterator, std::string()> link_rule;

	std::set<std::string> links;

	links_rule = *(qi::char_ - qi::lit('[')) >> 
		-(
			(&qi::lit("[[") >> link_rule [boost::phoenix::insert(boost::phoenix::ref(links), _1)] >> links_rule) 
			| (qi::lit('[') >> links_rule)
		);
	link_rule = qi::lit("[[") 
					>> +(!qi::lit("]]") >> (qi::char_ - qi::lit('|'))) [qi::_val += qi::_1]
					>> -(qi::lit('|') >> +(!qi::lit("]]") >> qi::char_)) 
					>> qi::lit("]]");

	auto it = data.Content.cbegin();
	qi::parse(it, data.Content.cend(), links_rule);

	for (auto link : links) 
	{
		boost::trim(link);

		auto target_it = std::find(_articles.begin(), _articles.end(), link);
		if(target_it != _articles.end())
		{
			target = target_it - _articles.begin();
		}
		else
			continue;

		_vecMutex.lock(source);
		_adjList[source].insert(target);
		_vecMutex.unlock(source);
	}
}
