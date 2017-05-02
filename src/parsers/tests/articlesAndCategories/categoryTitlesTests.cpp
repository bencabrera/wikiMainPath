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
#include "../../../core/wikiDataCache.h"
#include "../../helpers/sortTitlesHelper.h"
#include "../../wikiArticleHandlers/articleDatesAndCategoriesHandler.h"
#include "../../../../libs/wiki_xml_dump_xerces/src/parsers/singleCoreParser.hpp"
#include "../../../../libs/wiki_xml_dump_xerces/src/handlers/wikiDumpHandlerProperties.hpp"
#include "../../../../libs/wiki_xml_dump_xerces/src/handlers/basicTitleFilters.hpp"


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

	WikiXmlDumpXerces::WikiDumpHandlerProperties parser_properties;
	parser_properties.TitleFilter = WikiXmlDumpXerces::only_articles_and_categories();

	// init xerces
	xercesc::XMLPlatformUtils::Initialize();

	// run parsing
	auto article_titles_stream = std::make_shared<std::stringstream>();
	auto article_dates_stream = std::make_shared<std::stringstream>();
	auto category_titles_stream = std::make_shared<std::stringstream>();
	auto redirects_stream = std::make_shared<std::stringstream>();
	std::mutex article_titles_mutex;
	std::mutex category_titles_mutex;
	std::mutex redirect_mutex;
	ArticleDatesAndCategoriesHandler art_handler(*article_titles_stream, *article_dates_stream, *category_titles_stream, *redirects_stream, article_titles_mutex, category_titles_mutex, redirect_mutex);
	art_handler.ExtractOnlyArticlesWithDates = false;
	WikiXmlDumpXerces::SingleCoreParser parser(art_handler, parser_properties);
	parser.Run(path);

	article_titles_stream->seekg(0);
	article_dates_stream->seekg(0);
	category_titles_stream->seekg(0);
	redirects_stream->seekg(0);

	// terminate xerces
	xercesc::XMLPlatformUtils::Terminate();

	auto category_titles_sorted_stream = std::make_shared<std::stringstream>();
	sort_category_files(*category_titles_stream, *category_titles_sorted_stream);

	WikiMainPath::WikiDataCache wiki_data_cache(article_titles_stream, article_dates_stream, category_titles_sorted_stream, redirects_stream);
	auto& category_titles = wiki_data_cache.category_titles();

	std::size_t cat_idx = 0;
	bool foundCategory = getPosition(category_titles, "Friendly fire incidents of World War II", cat_idx);
	BOOST_CHECK(foundCategory);
	std::size_t cat_idx_recursive = 0;
	bool foundCategory2 = getPosition(category_titles, "World War II", cat_idx_recursive);
	BOOST_CHECK(foundCategory2);
}

BOOST_AUTO_TEST_SUITE_END()
