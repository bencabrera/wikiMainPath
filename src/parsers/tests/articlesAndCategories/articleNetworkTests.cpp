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

// wiki xml dump lib
#include "../../wikiArticleHandlers/articleDatesAndCategoriesHandler.h"
#include "../../wikiArticleHandlers/linkExtractionHandler.h"
#include "../../../../libs/wiki_xml_dump_xerces/src/parsers/singleCoreParser.hpp"
#include "../../../../libs/wiki_xml_dump_xerces/src/handlers/wikiDumpHandlerProperties.hpp"
#include "../../../../libs/wiki_xml_dump_xerces/src/handlers/basicTitleFilters.hpp"

BOOST_AUTO_TEST_SUITE(article_network_tests)

BOOST_AUTO_TEST_CASE(there_should_be_links_between_the_articles)
{
	std::string path("../../src/parsers/tests/articlesAndCategories/data/1789_events_french_revolution.xml");

	// init xerces
	xercesc::XMLPlatformUtils::Initialize();

	WikiXmlDumpXerces::WikiDumpHandlerProperties parser_properties;
	parser_properties.TitleFilter = WikiXmlDumpXerces::only_articles();

	// init xerces
	xercesc::XMLPlatformUtils::Initialize();

	// step 1: run parsing for article list
	ArticleDatesAndCategoriesHandler art_handler;
	art_handler.ExtractOnlyArticlesWithDates = false;
	WikiXmlDumpXerces::SingleCoreParser parser(art_handler, parser_properties);
	parser.Run(path);

	std::vector<std::string> articles;
	for (auto key_value : art_handler.articles) {
		articles.push_back(key_value.first);	
	}
	std::sort(articles.begin(), articles.end());

	// step 2: run parsing for article network
	VectorMutex<1000> vecMutex;
	std::vector<boost::container::flat_set<std::size_t>> adj_list(art_handler.articles.size());
	LinkExtractionHandler art_handler_link(articles, art_handler.redirects, adj_list, vecMutex);
	WikiXmlDumpXerces::SingleCoreParser parser2(art_handler_link, parser_properties);
	parser2.Run(path);

	// in this test the direction of the edge is determined by the lexicographic sort
	std::vector<std::pair<std::string,std::string>> expected_links = {
		{ "National Assembly (French Revolution)", "Storming of the Bastille" },
		{ "National Constituent Assembly", "Women's March on Versailles" },
		{ "August Decrees", "National Constituent Assembly" }
	};

	for (auto link : expected_links) {
		auto it_1 = std::find(articles.begin(), articles.end(), link.first);
		auto it_2 = std::find(articles.begin(), articles.end(), link.second);

		bool found_article1 = it_1 != articles.end();
		bool found_article2 = it_2 != articles.end();

		BOOST_CHECK(found_article1);
		BOOST_CHECK(found_article2);

		if(found_article1 && found_article2)
		{
			BOOST_CHECK(adj_list[it_1 - articles.begin()].find(it_2 - articles.begin()) != adj_list[it_1 - articles.begin()].end());
		}

	}
}

BOOST_AUTO_TEST_SUITE_END()
