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
	if(data.IsRedirect)
		return;
	
	std::string title = data.MetaData.at("title");
	boost::trim(title);

	bool is_article = true, is_category = false;
	if(title.length() > 9)
	{
		is_category = title.substr(0,9) == "Category:";
		is_article = !is_category;
	}

	auto links = parse_all_links(data.Content);

	// compute where the links to categories lie in links
	auto categories_begin_it = std::lower_bound(links.begin(), links.end(), "Category:");	
	auto categories_end_it = std::upper_bound(links.begin(), links.end(), "Category:zzzzzzzzzzzzzzzzzz");	

	// if it is a category => add link to category hirachy network
	if(is_category)
	{
		title = title.substr(9);
		std::size_t i_parent;
		if(!get_position(_category_titles, title, i_parent))
			return;
		
		for(auto it = categories_begin_it; it != categories_end_it; it++)
		{
			auto linked_category_title = it->substr(9);
			if(_redirects.count(linked_category_title) > 0)
				linked_category_title = _redirects.at(linked_category_title);

			std::size_t i_child;
			if(!get_position(_category_titles, linked_category_title, i_child))
				continue;

			std::size_t i_min = std::min(i_child % 1000, i_parent % 1000);
			std::size_t i_max = std::max(i_child % 1000, i_parent % 1000);
				
			if(i_min == i_max)
				_vecMutex.lock(i_min);
			else
			{
				_vecMutex.lock(i_min);
				_vecMutex.lock(i_max);
			}

			boost::add_edge(i_parent, i_child, _category_hirachy_graph);
			_vecMutex.unlock(i_max);
			_vecMutex.unlock(i_min);
		}

		return;
	}

	if(is_article)
	{
		std::size_t i_from_article;
		if(!get_position(_article_titles, title, i_from_article))
			return;

		// categories that this is part of
		for(auto it = categories_begin_it; it != categories_end_it; it++)
		{
			auto category_title = it->substr(9);

			if(_redirects.count(category_title) > 0)
				category_title = _redirects.at(category_title);

			std::size_t i_category;
			if(get_position(_category_titles, category_title, i_category))
			{
				_vecMutex.lock(i_category);
				_category_has_article[i_category].insert(i_from_article);			
				_vecMutex.unlock(i_category);
			}
		}


		// link to other articles (the following loop-bodies are equivalent)
		for(auto it = links.begin(); it != categories_begin_it; it++)
		{
			std::string article_title = *it;
			if(_redirects.count(article_title) > 0)
				article_title = _redirects.at(article_title);
			
			std::size_t i_to_article;
			if(!get_position(_article_titles, article_title, i_to_article))
				continue;

			_vecMutex.lock(i_from_article);
			_article_adjacency_list[i_from_article].insert(i_to_article);
			_vecMutex.unlock(i_from_article);
		}
		for(auto it = categories_end_it; it != links.end(); it++)
		{
			std::string article_title = *it;
			if(_redirects.count(article_title) > 0)
				article_title = _redirects.at(article_title);
			
			std::size_t i_to_article;
			if(!get_position(_article_titles, article_title, i_to_article))
				continue;

			_vecMutex.lock(i_from_article);
			_article_adjacency_list[i_from_article].insert(i_to_article);
			_vecMutex.unlock(i_from_article);
		}
	}

}
