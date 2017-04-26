#include <boost/test/unit_test.hpp>

#include <fstream>
#include <streambuf>
#include <string>
#include <mutex>
#include <sstream>

// xerces
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>

// wiki xml dump lib
#include "../../../core/wikiDataCache.h"
#include "../../sortTitlesHelper.h"
#include "../../wikiArticleHandlers/articleDatesAndCategoriesHandler.h"
#include "../../../../libs/wiki_xml_dump_xerces/src/parsers/singleCoreParser.hpp"
#include "../../../../libs/wiki_xml_dump_xerces/src/handlers/wikiDumpHandlerProperties.hpp"
#include "../../../../libs/wiki_xml_dump_xerces/src/handlers/basicTitleFilters.hpp"


BOOST_AUTO_TEST_SUITE(article_list_tests)

BOOST_AUTO_TEST_CASE(article_list_should_contain_all_articles)
{
	std::string path("../../src/parsers/tests/articlesAndCategories/data/friendly_fire_incidents_of_world_war_2.xml");

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

	auto article_titles_sorted_stream = std::make_shared<std::stringstream>();
	auto article_dates_sorted_stream = std::make_shared<std::stringstream>();
	sort_article_files(*article_titles_stream, *article_dates_stream, *article_titles_sorted_stream, *article_dates_sorted_stream);

	WikiMainPath::WikiDataCache wiki_data_cache(article_titles_sorted_stream, article_dates_sorted_stream, category_titles_stream, redirects_stream);
	auto& article_titles = wiki_data_cache.article_titles();

	std::set<std::string> articles(article_titles.begin(), article_titles.end());

	std::vector<std::string> expected_articles = {
		"Operation Scylla",
		"HMS Britomart (J22)",
		"Italian submarine Alagi",
		"HMS Bicester (L34)",
		"USS Brant (AM-24)",
		"USS Dorado (SS-248)",
		"USS Extractor (ARS-15)",
		"HMS Hussar (J82)",
		"HMS Jason (J99)",
		"ORP Jastrząb",
		"USS McCawley (APA-4)",
		"Motor Torpedo Boat PT-346",
		"HNoMS Nordkapp",
		"HMS Oxley",
		"USS Porter (DD-356)",
		"Motor Torpedo Boat PT-121",
		"USS R-19 (SS-96)",
		"HMS Sunfish (81S)",
		"HMS Unbeaten",
		"German destroyer Z1 Leberecht Maass",
		"German destroyer Z3 Max Schultz",
		"German submarine U-557",
		"Imber friendly fire incident",
		"Operation Wikinger",
		"USS Albert W. Grant (DD-649)",
		"German submarine U-235",
		"USS Seawolf (SS-197)",
		"USS Atlanta (CL-51)",
		"Exercise Tiger",
		"Operation Cottage",
		"Battle of Barking Creek",
		"Battle of Los Angeles"
	};

	for (auto ex_art : expected_articles) 
		BOOST_CHECK(articles.find(ex_art) != articles.end());
}

BOOST_AUTO_TEST_SUITE_END()
