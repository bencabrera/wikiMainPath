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
#include "../../s3_recursiveFillCategories.h"
#include "../helpers/buildArtificialDump.h"
#include "../../../core/wikiDataCache.h"
#include "../../sortTitlesHelper.h"
#include "../../wikiArticleHandlers/articleDatesAndCategoriesHandler.h"
#include "../../wikiArticleHandlers/allLinksArticleHandler.h"
#include "../../../../libs/wiki_xml_dump_xerces/src/parsers/singleCoreParser.hpp"
#include "../../../../libs/wiki_xml_dump_xerces/src/handlers/wikiDumpHandlerProperties.hpp"
#include "../../../../libs/wiki_xml_dump_xerces/src/handlers/basicTitleFilters.hpp"

BOOST_AUTO_TEST_SUITE(category_has_article_tests)

BOOST_AUTO_TEST_CASE(check_if_recursive_articles_are_detected_articial)
{
	std::vector<std::string> titles = { "A", "B", "C", "D", "E", "Category:C1", "Category:C2", "Category:C3" };
	std::vector<std::pair<std::string,std::string>> expected_links = {
		{ "A", "C" },
		{ "A", "D" },
		{ "C", "D" },
		{ "D", "E" },
		{ "D", "A" },
		{ "D", "C" },
		{ "E", "A" },
		{ "A", "Category:C3" },
		{ "B", "Category:C3" },
		{ "C", "Category:C2" },
		{ "D", "Category:C1" },
		{ "Category:C2", "Category:C1" },
		{ "Category:C3", "Category:C2" },
	};

	std::vector<std::pair<std::string,std::string>> expected_category_containment = {
		{ "A", "C1" },
		{ "A", "C2" },
		{ "A", "C3" },
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

	recursiveFillCategories(category_hirachy_graph, category_has_article);	

	for (auto containment : expected_category_containment) {
		auto it_1 = std::find(article_titles.begin(), article_titles.end(), containment.first);
		auto it_2 = std::find(category_titles.begin(), category_titles.end(), containment.second);

		bool found_article = it_1 != article_titles.end();
		bool found_category= it_2 != category_titles.end();

		BOOST_CHECK(found_article);
		BOOST_CHECK(found_category);

		std::size_t i_article = it_1-article_titles.begin();
		std::size_t i_category = it_2-category_titles.begin();

		if(found_article && found_category)
		{
			bool found = std::find(category_has_article[i_category].begin(), category_has_article[i_category].end(), i_article) != category_has_article[i_category].end();
			if(!found)
				std::cout << containment.first << " - " << containment.second << std::endl;
			BOOST_CHECK(found);
		}

	}
}

BOOST_AUTO_TEST_SUITE_END()
