#pragma once
#include <boost/test/included/unit_test.hpp>

#include <fstream>
#include <streambuf>
#include <string>

// xerces
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>

#include "../xml/wikiDumpHandler.h"
#include "../wikiHandlers/articleDatesAndCategoriesHandler.h"
#include "../shared.h"
#include "../parserWrappers/s1_wrapper.h"

BOOST_AUTO_TEST_SUITE(ArticleListTests)

BOOST_AUTO_TEST_CASE(ArticleListShouldContainTheRightArticlesExample1)
{
	fs::path path("../../src/parsers/tests/data/friendly_fire_incidents_of_world_war_2.xml");

	// init xerces
	xercesc::XMLPlatformUtils::Initialize();

	std::map<std::string, std::size_t> pageCounts;
	S1ParserWrapper s1_wrapper(path, pageCounts, false);
	s1_wrapper.ProgressCallback = std::function<void(std::size_t)>();
	auto artHandler = s1_wrapper();

	std::map<std::string,Date> articlesWithDates = artHandler.articles;

	std::vector<std::string> expected_articles = {
		"Operation Scylla",
		"Exercise Tiger",
		"Operation Cottage",
		"Battle of Barking Creek",
		"Battle of Los Angeles"
	};

	for (auto ex_art : expected_articles) 
		BOOST_CHECK(articlesWithDates.find(ex_art) != articlesWithDates.end());
}

BOOST_AUTO_TEST_SUITE_END()
