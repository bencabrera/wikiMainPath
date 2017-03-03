cmake_minimum_required(VERSION 3.1)
project(WikiMainPath)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

# --- Boost library setup ---
# set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.54 REQUIRED COMPONENTS filesystem system program_options graph regex)

# --- Core library ---
set(CORE_FILES
	core/date.cpp
	core/wikiDataCache.cpp
)

add_library(CoreLib SHARED ${CORE_FILES})




# --- Parsers ---
set(PARSER_FILES
	# handlers
	# parsers/wikiArticleHandlers/articleTitlesHandler.cpp
	parsers/wikiArticleHandlers/articleDatesAndCategoriesHandler.cpp
	parsers/wikiArticleHandlers/countArticleLengthsHandler.cpp
	parsers/wikiArticleHandlers/categoryHasArticleHandler.cpp
	parsers/wikiArticleHandlers/categoryRecursiveHandler.cpp
	parsers/wikiArticleHandlers/linkExtractionHandler.cpp

	# date extraction
	parsers/date/articleDateExtraction.cpp
	parsers/date/infoboxDateExtraction.cpp
	parsers/date/stringDateExtraction.cpp

	# date grammars
	parsers/date/grammars/dateStringGrammar.cpp
	parsers/date/grammars/wikiDateTemplateGrammar.cpp
	parsers/date/grammars/wikiFuzzyDateTemplateGrammar.cpp
	parsers/date/grammars/infoboxKeyValueGrammar.cpp

	parsers/s3_recursiveFillCategories.cpp
	parsers/s3_cycle_detector_visitor.cpp
	parsers/shared.h
	parsers/shared.cpp
)

add_library(ParserLib SHARED ${PARSER_FILES})

# parser executables
add_executable(countArticleLengths parsers/main_countArticleLengths.cpp)
target_link_libraries(countArticleLengths ParserLib)
target_link_libraries(countArticleLengths ${Boost_LIBRARIES})
target_link_libraries(countArticleLengths xerces-c)
target_link_libraries(countArticleLengths pthread)

add_executable(s1_articlesAndCategories parsers/main_s1_articlesAndCategories.cpp)
target_link_libraries(s1_articlesAndCategories CoreLib)
target_link_libraries(s1_articlesAndCategories ParserLib)
target_link_libraries(s1_articlesAndCategories ${Boost_LIBRARIES})
target_link_libraries(s1_articlesAndCategories xerces-c)
target_link_libraries(s1_articlesAndCategories pthread)

add_executable(s2_categoryHasArticles parsers/main_s2_categoryHasArticles.cpp)
target_link_libraries(s2_categoryHasArticles ParserLib)
target_link_libraries(s2_categoryHasArticles ${Boost_LIBRARIES})
target_link_libraries(s2_categoryHasArticles xerces-c)
target_link_libraries(s2_categoryHasArticles pthread)

add_executable(s3_recursiveCategories parsers/main_s3_recursiveCategories.cpp)
target_link_libraries(s3_recursiveCategories ParserLib)
target_link_libraries(s3_recursiveCategories ${Boost_LIBRARIES})
target_link_libraries(s3_recursiveCategories xerces-c)
target_link_libraries(s3_recursiveCategories pthread)

add_executable(s4_articleNetwork parsers/main_s4_articleNetwork.cpp)
target_link_libraries(s4_articleNetwork ${Boost_LIBRARIES})
target_link_libraries(s4_articleNetwork CoreLib)
target_link_libraries(s4_articleNetwork ParserLib)
target_link_libraries(s4_articleNetwork xerces-c)
target_link_libraries(s4_articleNetwork pthread)

# add_executable(countPages parsers/main_countPages.cpp)
# target_link_libraries(countPages ParserLib)
# target_link_libraries(countPages ${Boost_LIBRARIES})
# target_link_libraries(countPages xerces-c)
# target_link_libraries(countPages pthread)




# --- Parser tests ---
set(PARSER_TEST_FILES
	parsers/tests/articlesAndCategories/articleNetworkTests.cpp
	# parsers/tests/articlesAndCategories/categoryTests.cpp
	parsers/tests/articlesAndCategories/articleListTests.cpp
	parsers/tests/articlesAndCategories/cycleDectionTests.cpp
	parsers/tests/date/dateModelTests.cpp
	parsers/tests/date/infoboxDateExtractionTests.cpp
	parsers/tests/date/articleDateExtractionTests.cpp
	parsers/tests/date/dateStringGrammarTests.cpp
	parsers/tests/date/wikiDateTemplateGrammarTests.cpp
	parsers/tests/date/wikiFuzzyDateTemplateGrammarTests.cpp
)

add_library(ParserTestsLib SHARED ${PARSER_TEST_FILES})

add_executable(parserTests parsers/tests/main_tests.cpp)
target_link_libraries(parserTests ${Boost_LIBRARIES})
target_link_libraries(parserTests CoreLib)
target_link_libraries(parserTests ParserLib)
target_link_libraries(parserTests ParserTestsLib)
target_link_libraries(parserTests xerces-c)
target_link_libraries(parserTests ${Boost_LIBRARIES})
target_link_libraries(parserTests ParserLib)
target_link_libraries(parserTests CoreLib)

# --- backend server ---
set(BACKEND_FILES
	backend/routingHandlerFactory.cpp
	backend/fullTextSearch.cpp
	backend/serverData.cpp
	backend/httpRequestHandlers/mpaHandler.cpp
	backend/httpRequestHandlers/searchHandler.cpp
	backend/httpRequestHandlers/notFoundHandler.cpp
	backend/httpRequestHandlers/articleListHandler.cpp
	backend/httpRequestHandlers/articleNetworkHandler.cpp
)

set(PORTER_2_STEMMER_FILES
	../libs/porter2_stemmer/porter2_stemmer.h
	../libs/porter2_stemmer/porter2_stemmer.cpp
	../libs/porter2_stemmer/util/string_view.h
	../libs/porter2_stemmer/util/hash.h
)

add_library(BackendLib SHARED ${BACKEND_FILES})
add_library(Porter2_Stemmer SHARED ${PORTER_2_STEMMER_FILES})

add_executable(backend backend/main_backend.cpp)
target_link_libraries(backend BackendLib)
target_link_libraries(backend CoreLib)
target_link_libraries(backend ${Boost_LIBRARIES})
target_link_libraries(backend Porter2_Stemmer)
target_link_libraries(backend PocoNet)
target_link_libraries(backend PocoFoundation)
target_link_libraries(backend PocoUtil)
target_link_libraries(backend PocoJSON)
target_link_libraries(backend pthread)
