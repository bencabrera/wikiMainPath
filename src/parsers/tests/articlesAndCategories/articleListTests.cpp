#include <boost/test/unit_test.hpp>

#include <fstream>
#include <streambuf>
#include <string>

// xerces
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>

// wiki xml dump lib
#include "../../wikiArticleHandlers/articleDatesAndCategoriesHandler.h"
#include "../../../../libs/wiki_xml_dump_xerces/src/parsers/singleCoreParser.hpp"
#include "../../../../libs/wiki_xml_dump_xerces/src/handlers/wikiDumpHandlerProperties.hpp"


BOOST_AUTO_TEST_SUITE(article_list_tests)

BOOST_AUTO_TEST_CASE(article_list_should_contain_all_articles)
{
	std::string path("../../src/parsers/tests/articlesAndCategories/data/friendly_fire_incidents_of_world_war_2.xml");

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

	// init xerces
	xercesc::XMLPlatformUtils::Initialize();

	// run parsing
	ArticleDatesAndCategoriesHandler art_handler;
	art_handler.ExtractOnlyArticlesWithDates = false;
	WikiXmlDumpXerces::SingleCoreParser parser(art_handler, parser_properties);
	parser.Run(path);

	// terminate xerces
	xercesc::XMLPlatformUtils::Terminate();

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
		BOOST_CHECK(art_handler.articles.find(ex_art) != art_handler.articles.end());
}

BOOST_AUTO_TEST_SUITE_END()
