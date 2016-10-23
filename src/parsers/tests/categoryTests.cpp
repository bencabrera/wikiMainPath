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

#include "../shared.h"

#include "../parserWrappers/s1_wrapper.h"
#include "../parserWrappers/s2_wrapper.h"
#include "../parserWrappers/s3_wrapper.h"
#include "../parserWrappers/s3_recursiveFillCategories.h"

BOOST_AUTO_TEST_SUITE(CategoryTests)

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



BOOST_AUTO_TEST_CASE(CategoryShouldContainArticlesExample1)
{
	fs::path path("../../src/parsers/tests/data/recursive_category_20th_century_conflicts.xml");

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

	std::vector<std::string> categories = artHandler.categories;
	std::sort(categories.begin(), categories.end());
	std::map<std::string, std::string> redirects = artHandler.redirects;
	VectorMutex<1000> vecMutex;

	std::vector<boost::container::flat_set<std::size_t>> category_has_article(categories.size());

	S2ParserWrapper s2_wrapper(path, pageCounts, articles, categories, category_has_article, vecMutex);
	s2_wrapper.ProgressCallback = std::function<void(std::size_t)>();
	s2_wrapper();

	std::size_t cat_idx = 0;
	bool foundCategory = getPosition(categories, "Friendly fire incidents of World War II", cat_idx);

	BOOST_CHECK(foundCategory);

	std::vector<std::string> articles_belonging_to_category = {
		"Operation Scylla",
		"Exercise Tiger",
		"Operation Cottage",
		"Battle of Barking Creek",
		"Battle of Los Angeles"
	};

	for (auto art : articles_belonging_to_category) {
		std::size_t art_idx = 0;
		bool found_article = getPosition(articles, art, art_idx);
		BOOST_CHECK(found_article);

		BOOST_CHECK(category_has_article[cat_idx].find(art_idx) != category_has_article[cat_idx].end());
	}


	S3ParserWrapper::Graph graph(categories.size());
	S3ParserWrapper s3_wrapper(path, pageCounts, categories, graph, vecMutex);
	s3_wrapper.ProgressCallback = std::function<void(std::size_t)>();
	s3_wrapper();

	recursiveFillCategories(graph, category_has_article);

	std::size_t cat_idx_recursive = 0;
	bool foundCategory2 = getPosition(categories, "World War II", cat_idx_recursive);

	BOOST_CHECK(foundCategory2);

	for (auto art : articles_belonging_to_category) {
		std::size_t art_idx = 0;
		bool found_article = getPosition(articles, art, art_idx);
		BOOST_CHECK(found_article);

		BOOST_CHECK(category_has_article[cat_idx_recursive].find(art_idx) != category_has_article[cat_idx_recursive].end());
	}
}

BOOST_AUTO_TEST_SUITE_END()
