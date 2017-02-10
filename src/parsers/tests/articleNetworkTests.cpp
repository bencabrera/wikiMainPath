#include <boost/test/included/unit_test.hpp>

#include <fstream>
#include <streambuf>
#include <string>

// xerces
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>

#include "../shared.h"

#include "../parserWrappers/s1_wrapper.h"
#include "../parserWrappers/s4_wrapper.h"

BOOST_AUTO_TEST_SUITE(ArticleNetworkTests)


BOOST_AUTO_TEST_CASE(ThereShouldBeLinkBetweenArticles1)
{
	fs::path path("../../src/parsers/tests/data/1789_events_french_revolution.xml");

	// init xerces
	xercesc::XMLPlatformUtils::Initialize();

	std::map<std::string, std::size_t> pageCounts;
	S1ParserWrapper s1_wrapper(path, pageCounts, false);
	s1_wrapper.ProgressCallback = std::function<void(std::size_t)>();
	auto artHandler = s1_wrapper();

	std::vector<std::string> articles;
	std::vector<Date> dates;
	for (auto el : artHandler.articles) {
		articles.push_back(el.first);	
		dates.push_back(el.second);	
	}

	std::map<std::string, std::string> redirects = artHandler.redirects;
	VectorMutex<1000> vecMutex;

	std::vector<boost::container::flat_set<std::size_t>> adjList(articles.size());
	S4ParserWrapper s4_wrapper(path, pageCounts, articles, dates, redirects, adjList, vecMutex);
	s4_wrapper.ProgressCallback = std::function<void(std::size_t)>();
	s4_wrapper.OrderCallback = [](std::size_t source, std::size_t target)
	{
		return source < target;
	};
	s4_wrapper();

	std::vector<std::pair<std::string,std::string>> expected_links = {
		{ "National Assembly (French Revolution)", "Storming of the Bastille" },
		{ "National Constituent Assembly", "Women's March on Versailles" },
		{ "August Decrees", "National Constituent Assembly" }
	};

	for (auto link : expected_links) {
		std::size_t from_idx = 0, to_idx = 0;
		bool found_article1 = getPosition(articles, link.first, from_idx);
		bool found_article2 = getPosition(articles, link.second, to_idx);
		BOOST_CHECK(found_article1);
		BOOST_CHECK(found_article2);

		BOOST_CHECK(adjList[from_idx].find(to_idx) != adjList[from_idx].end());
	}
}

BOOST_AUTO_TEST_SUITE_END()
