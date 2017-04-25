#include "allLinksArticleHandler.h"

#include <iostream>
#include <boost/algorithm/string/trim.hpp>

// boost spirit 
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_repeat.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

AllLinksArticleHander::AllLinksArticleHander(
	const std::vector<std::string>& article_titles, 
	const std::vector<std::string>& category_titles, 
	const std::map<std::string, std::string>& redirects,
	std::vector<boost::container::flat_set<std::size_t>>& category_has_article, 
	CategoryHirachyGraph& category_hirachy_graph,
	std::vector<boost::container::flat_set<std::size_t>>& article_adjacency_list,
	VectorMutex<1000>& vecMut
)
:_article_titles(article_titles),
_category_titles(category_titles),
_redirects(redirects),
_category_has_article(category_has_article),
_category_hirachy_graph(category_hirachy_graph),
_article_adjacency_list(article_adjacency_list),
_vecMutex(vecMut)
{
	if(category_has_article.size() != category_titles.size())
		throw std::logic_error("'category_has_article' has to have the same size as 'category_titles'.");

	if(article_titles.size() != article_adjacency_list.size())
		throw std::logic_error("'article_titles' has to have the same size as 'article_adjacency_list'.");

	if(category_titles.size() != boost::num_vertices(category_hirachy_graph))
		throw std::logic_error("'category_titles' has to have the same size as the number of vertices inf 'category_hirachy_graph'.");
}

std::vector<std::string> AllLinksArticleHander::parse_all_links(const std::string& content)
{
	using namespace boost::spirit::qi;

	rule<std::string::const_iterator, std::vector<std::string>()> links_rule;
	rule<std::string::const_iterator, std::string()> link_rule;

	std::set<std::string> links;

	links_rule = *(char_ - lit('[')) >> 
		-(
			(&lit("[[") >> link_rule [boost::phoenix::insert(boost::phoenix::ref(links), _1)] >> links_rule) 
			| (lit('[') >> links_rule)
		);
	link_rule = lit("[[") 
					>> +(!lit("]]") >> (char_ - lit('|'))) [_val += _1]
					>> -(lit('|') >> +(!lit("]]") >> char_)) 
					>> lit("]]");

	auto it = content.cbegin();
	parse(it, content.cend(), links_rule);

	return std::vector<std::string>(links.begin(), links.end());
}


void AllLinksArticleHander::HandleArticle(const WikiXmlDumpXerces::WikiPageData& data)
{
	std::string title = data.MetaData.at("title");
	boost::trim(title);

	bool is_article = true, is_category = false;
	if(title.length() > 9)
	{
		is_category = title.substr(0,9) == "Category:";
		is_article = !is_category;
	}

	std::size_t articleIdx;
	if(!getPosition(articles, title, articleIdx))
		return;

	if(data.IsRedirect)
		return;

	// run through categories and add article to this categories
	std::size_t foundPos = data.Content.find("[[Category:");
	while(foundPos != std::string::npos)
	{
		std::size_t secondFoundPos = std::min(data.Content.find("]]", foundPos), data.Content.find("|", foundPos));
		if(secondFoundPos == std::string::npos)
			break;

		std::string category_title = data.Content.substr(foundPos + 11, secondFoundPos - foundPos - 11);
		boost::trim(category_title);

		std::size_t categoryIdx;
		if(getPosition(categories, category_title, categoryIdx))
		{
			_vecMutex.lock(categoryIdx);
			categoryHasArticle[categoryIdx].insert(articleIdx);			
			_vecMutex.unlock(categoryIdx);
		}

		foundPos = data.Content.find("[[Category:", secondFoundPos);
	}
}
