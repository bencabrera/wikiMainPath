#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <fstream>
#include <streambuf>
#include <string>

// xerces
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>

// wiki xml dump lib
#include "../../wikiArticleHandlers/articleDatesAndCategoriesHandler.h"
#include "../../wikiArticleHandlers/categoryRecursiveHandler.h"
#include "../../wikiArticleHandlers/categoryHasArticleHandler.h"
#include "../../../../libs/wiki_xml_dump_xerces/src/parsers/singleCoreParser.hpp"
#include "../../../../libs/wiki_xml_dump_xerces/src/handlers/wikiDumpHandlerProperties.hpp"
#include "../../s3_recursiveFillCategories.h"


BOOST_AUTO_TEST_SUITE(category_parsing_tests)

bool getPosition(const std::vector<std::string>& vec, std::string str, std::size_t& pos)
{
	auto it = std::lower_bound(vec.begin(), vec.end(), str);	
	if(it == vec.end() || *it != str)
		return false;
	else
	{
		pos = it - vec.begin();
		return true;
	}
}


BOOST_AUTO_TEST_CASE(recursive_category_should_contain_all_articles)
{
	std::string path("../../src/parsers/tests/articlesAndCategories/data/recursive_category_20th_century_conflicts.xml");

	// defined all objects that need to be filled during this test
	std::vector<std::string> articles;
	std::vector<Date> dates;
	std::vector<std::string> categories;
	std::vector<boost::container::flat_set<std::size_t>> category_has_article(categories.size());
	std::map<std::string, std::string> redirects;

	VectorMutex<1000> vec_mutex;

	// init xerces
	xercesc::XMLPlatformUtils::Initialize();

	WikiXmlDumpXerces::WikiDumpHandlerProperties parser_properties;
	parser_properties.TitleFilter = [](const std::string& title) {
		return !(
				title.substr(0,5) == "User:" 
				|| title.substr(0,10) == "Wikipedia:" 
				|| title.substr(0,5) == "File:" 
				|| title.substr(0,14) == "Category talk:" 
				|| title.substr(0,14) == "Template talk:"
				|| title.substr(0,9) == "Template:"
				|| title.substr(0,10) == "User talk:"
				|| title.substr(0,10) == "File talk:"
				|| title.substr(0,15) == "Wikipedia talk:"
				);
	};


	// step 1 - extract all articles, categories, redirects, etc... from the xml file
	ArticleDatesAndCategoriesHandler art_handler;
	art_handler.ExtractOnlyArticlesWithDates = false;
	WikiXmlDumpXerces::SingleCoreParser parser(art_handler, parser_properties);
	parser.Run(path);

	categories = art_handler.categories;
	redirects = art_handler.redirects;

	for (auto el : art_handler.articles) {
		articles.push_back(el.first);	
		dates.push_back(el.second);	
	}

	std::sort(categories.begin(), categories.end());

	// // step 2 - extract the "category includes article" relation
	// CategoryHasArticleHandler art_handler2(articles, categories, category_has_article, vec_mutex);
	// WikiXmlDumpXerces::SingleCoreParser parser2(art_handler2, parser_properties);
	// parser2.Run(path);

	// // check if the wanted categories have been found
	// std::size_t cat_idx = 0;
	// bool foundCategory = getPosition(categories, "Friendly fire incidents of World War II", cat_idx);
	// BOOST_CHECK(foundCategory);
	// std::size_t cat_idx_recursive = 0;
	// bool foundCategory2 = getPosition(categories, "World War II", cat_idx_recursive);
	// BOOST_CHECK(foundCategory2);

	// // check for a list of known articles in category if they have been extracted
	// std::vector<std::string> articles_belonging_to_category = {
		// "Operation Scylla",
		// "Exercise Tiger",
		// "Operation Cottage",
		// "Battle of Barking Creek",
		// "Battle of Los Angeles"
	// };

	// for (auto art : articles_belonging_to_category) {
		// std::size_t art_idx = 0;
		// bool found_article = getPosition(articles, art, art_idx);
		// BOOST_CHECK(found_article);
		// BOOST_CHECK(category_has_article[cat_idx].find(art_idx) != category_has_article[cat_idx].end());
	// }


	// // step 3 - recursively add categories
	// parser_properties.TitleFilter = [](const std::string& title) {
		// return title.substr(0,9) == "Category:";
	// };

	// typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, boost::no_property, boost::no_property, boost::vecS> Graph;
	// Graph graph(categories.size());
	// CategoryRecursiveHandler art_handler3(categories, graph, vec_mutex);
	// WikiXmlDumpXerces::SingleCoreParser parser3(art_handler3 ,parser_properties);
	// parser3.Run(path);

	// recursiveFillCategories(graph, category_has_article);

	// // check if all the known articles in the subcategory are now also in the parent category
	// for (auto art : articles_belonging_to_category) {
		// std::size_t art_idx = 0;
		// bool found_article = getPosition(articles, art, art_idx);
		// BOOST_CHECK(found_article);

		// BOOST_CHECK(category_has_article[cat_idx_recursive].find(art_idx) != category_has_article[cat_idx_recursive].end());
	// }
}

BOOST_AUTO_TEST_SUITE_END()
