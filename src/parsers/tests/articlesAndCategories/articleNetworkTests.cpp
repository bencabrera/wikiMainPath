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
#include "../helpers/buildArtificialDump.h"
#include "../../../core/wikiDataCache.h"
#include "../../helpers/sortTitlesHelper.h"
#include "../../wikiArticleHandlers/articleDatesAndCategoriesHandler.h"
#include "../../wikiArticleHandlers/allLinksArticleHandler.h"
#include "../../../../libs/wiki_xml_dump_xerces/src/parsers/singleCoreParser.hpp"
#include "../../../../libs/wiki_xml_dump_xerces/src/handlers/wikiDumpHandlerProperties.hpp"
#include "../../../../libs/wiki_xml_dump_xerces/src/handlers/basicTitleFilters.hpp"

BOOST_AUTO_TEST_SUITE(article_network_tests)

BOOST_AUTO_TEST_CASE(there_should_be_links_between_the_articles)
{
	std::string path("../../src/parsers/tests/articlesAndCategories/data/1789_events_french_revolution.xml");

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

	auto article_titles_sorted_stream = std::make_shared<std::stringstream>();
	auto article_dates_sorted_stream = std::make_shared<std::stringstream>();
	auto category_titles_sorted_stream = std::make_shared<std::stringstream>();
	sort_article_files(*article_titles_stream, *article_dates_stream, *article_titles_sorted_stream, *article_dates_sorted_stream);
	sort_category_files(*category_titles_stream, *category_titles_sorted_stream);

	WikiMainPath::WikiDataCache wiki_data_cache(article_titles_sorted_stream, article_dates_sorted_stream, category_titles_stream, redirects_stream);
	auto& article_titles = wiki_data_cache.article_titles();
	auto& category_titles = wiki_data_cache.category_titles();
	auto& redirects = wiki_data_cache.redirects();
	std::vector<boost::container::flat_set<std::size_t>> category_has_article(category_titles.size());
	AllLinksArticleHander::CategoryHirachyGraph category_hirachy_graph(category_titles.size());
	std::vector<boost::container::flat_set<std::size_t>> article_adjacency_list(article_titles.size());
	VectorMutex<1000> vector_mutex;

	AllLinksArticleHander all_links_handler(article_titles, category_titles, redirects, category_has_article, category_hirachy_graph, article_adjacency_list, vector_mutex);
	WikiXmlDumpXerces::SingleCoreParser parser2(all_links_handler, parser_properties);
	parser2.Run(path);

	xercesc::XMLPlatformUtils::Terminate();

	// in this test the direction of the edge is determined by the lexicographic sort
	std::vector<std::pair<std::string,std::string>> expected_links = {
		{ "National Assembly (French Revolution)", "Storming of the Bastille" },
		{ "Women's March on Versailles", "National Constituent Assembly" },
		{ "August Decrees", "National Constituent Assembly" }
	};

	for (auto link : expected_links) {
		auto it_1 = std::find(article_titles.begin(), article_titles.end(), link.first);
		auto it_2 = std::find(article_titles.begin(), article_titles.end(), link.second);

		bool found_article1 = it_1 != article_titles.end();
		bool found_article2 = it_2 != article_titles.end();

		BOOST_CHECK(found_article1);
		BOOST_CHECK(found_article2);

		if(found_article1 && found_article2)
		{
			bool found_link = article_adjacency_list[it_1 - article_titles.begin()].find(it_2 - article_titles.begin()) != article_adjacency_list[it_1 - article_titles.begin()].end();
			if(!found_link)
				std::cout << link.first << " - " << link.second << std::endl;
			BOOST_CHECK(found_link);
		}

	}
}




BOOST_AUTO_TEST_CASE(there_should_be_links_between_the_articles_articial)
{
	std::vector<std::string> titles = { "A", "B", "C", "D", "E" };
	std::vector<std::pair<std::string,std::string>> expected_links = {
		{ "A", "C" },
		{ "A", "D" },
		{ "C", "D" },
		{ "D", "E" },
		{ "D", "A" },
		{ "D", "C" },
		{ "E", "A" },
	};
	std::string content = build_artificial_dump(titles, expected_links);

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
	parser.RunOnString(content);

	article_titles_stream->seekg(0);
	article_dates_stream->seekg(0);
	category_titles_stream->seekg(0);
	redirects_stream->seekg(0);

	// terminate xerces

	auto article_titles_sorted_stream = std::make_shared<std::stringstream>();
	auto article_dates_sorted_stream = std::make_shared<std::stringstream>();
	auto category_titles_sorted_stream = std::make_shared<std::stringstream>();
	sort_article_files(*article_titles_stream, *article_dates_stream, *article_titles_sorted_stream, *article_dates_sorted_stream);
	sort_category_files(*category_titles_stream, *category_titles_sorted_stream);

	article_titles_sorted_stream->seekg(0);
	category_titles_sorted_stream->seekg(0);

	WikiMainPath::WikiDataCache wiki_data_cache(article_titles_sorted_stream, article_dates_sorted_stream, category_titles_sorted_stream, redirects_stream);
	auto& article_titles = wiki_data_cache.article_titles();
	auto& category_titles = wiki_data_cache.category_titles();
	auto& redirects = wiki_data_cache.redirects();
	std::vector<boost::container::flat_set<std::size_t>> category_has_article(category_titles.size());
	AllLinksArticleHander::CategoryHirachyGraph category_hirachy_graph(category_titles.size());
	std::vector<boost::container::flat_set<std::size_t>> article_adjacency_list(article_titles.size());
	VectorMutex<1000> vector_mutex;

	AllLinksArticleHander all_links_handler(article_titles, category_titles, redirects, category_has_article, category_hirachy_graph, article_adjacency_list, vector_mutex);
	WikiXmlDumpXerces::SingleCoreParser parser2(all_links_handler, parser_properties);
	parser2.RunOnString(content);

	xercesc::XMLPlatformUtils::Terminate();

	for (auto link : expected_links) {
		auto it_1 = std::find(article_titles.begin(), article_titles.end(), link.first);
		auto it_2 = std::find(article_titles.begin(), article_titles.end(), link.second);

		bool found_article1 = it_1 != article_titles.end();
		bool found_article2 = it_2 != article_titles.end();

		BOOST_CHECK(found_article1);
		BOOST_CHECK(found_article2);

		if(found_article1 && found_article2)
		{
			bool found_link = article_adjacency_list[it_1 - article_titles.begin()].find(it_2 - article_titles.begin()) != article_adjacency_list[it_1 - article_titles.begin()].end();
			if(!found_link)
				std::cout << link.first << " - " << link.second << std::endl;
			BOOST_CHECK(found_link);
		}

	}
}

BOOST_AUTO_TEST_SUITE_END()
