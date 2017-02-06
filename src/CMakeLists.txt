cmake_minimum_required(VERSION 3.1)
project(WikiMainPath)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror")

# Boost stuff
# WARNING: Boost needs to be build using -fPIC option
set(BOOST_ROOT /usr/local/include/boost/)
set(BOOST_LIBRARYDIR /usr/local/lib/)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.54 REQUIRED COMPONENTS filesystem system program_options graph regex)

include_directories(${Boost_INCLUDE_DIRS})

set(PARSER_FILES
	parsers/wikiArticleHandlers/articleTitlesHandler.h
	parsers/wikiArticleHandlers/articleTitlesHandler.cpp
	parsers/wikiArticleHandlers/articleDatesAndCategoriesHandler.h
	parsers/wikiArticleHandlers/articleDatesAndCategoriesHandler.cpp
	parsers/wikiArticleHandlers/countArticleLengthsHandler.h
	parsers/wikiArticleHandlers/countArticleLengthsHandler.cpp
	parsers/wikiArticleHandlers/categoryHasArticleHandler.h
	parsers/wikiArticleHandlers/categoryHasArticleHandler.cpp
	parsers/wikiArticleHandlers/categoryRecursiveHandler.h
	parsers/wikiArticleHandlers/categoryRecursiveHandler.cpp
	parsers/wikiArticleHandlers/linkExtractionHandler.h
	parsers/wikiArticleHandlers/linkExtractionHandler.cpp
	parsers/date/dateExtraction.h
	parsers/date/dateExtraction.cpp
	parsers/date/dateStringGrammar.cpp
	parsers/date/infoboxGrammar.cpp
	parsers/date/infoboxKeyValueGrammar.cpp
	parsers/s3_recursiveFillCategories.cpp
	parsers/s3_cycle_detector_visitor.cpp
	parsers/shared.h
	parsers/shared.cpp
)

set(BACKEND_FILES
	backend/routingHandlerFactory.cpp
	backend/fullTextSearch.cpp
	backend/serverData.cpp
	backend/httpRequestHandlers/mpaRequestHandler.cpp
	backend/httpRequestHandlers/searchCategoryHandler.cpp
	backend/httpRequestHandlers/notFoundHandler.cpp
)

set(CORE_FILES
	core/date.cpp
)

set(PORTER_2_STEMMER_FILES
	../libs/porter2_stemmer/porter2_stemmer.h
	../libs/porter2_stemmer/porter2_stemmer.cpp
	../libs/porter2_stemmer/util/string_view.h
	../libs/porter2_stemmer/util/hash.h
)

# libraries
add_library(CoreLib STATIC ${CORE_FILES})
add_library(ParserLib STATIC ${PARSER_FILES})
add_library(BackendLib STATIC ${BACKEND_FILES})
add_library(Porter2_Stemmer STATIC ${PORTER_2_STEMMER_FILES})

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
target_link_libraries(s4_articleNetwork CoreLib)
target_link_libraries(s4_articleNetwork ParserLib)
target_link_libraries(s4_articleNetwork ${Boost_LIBRARIES})
target_link_libraries(s4_articleNetwork xerces-c)
target_link_libraries(s4_articleNetwork pthread)

# add_executable(countPages parsers/main_countPages.cpp)
# target_link_libraries(countPages ParserLib)
# target_link_libraries(countPages ${Boost_LIBRARIES})
# target_link_libraries(countPages xerces-c)
# target_link_libraries(countPages pthread)

add_executable(parserTests parsers/tests/main_tests.cpp)
target_link_libraries(parserTests CoreLib)
target_link_libraries(parserTests ParserLib)
target_link_libraries(parserTests ${Boost_LIBRARIES})
target_link_libraries(parserTests xerces-c)

# backend executables
find_library(POCO_LIB NAMES libPocoJSON.a libPocoFoundation.a libPocoNet.a)

add_executable(backend backend/main_backend.cpp)
target_link_libraries(backend BackendLib)
target_link_libraries(backend CoreLib)
target_link_libraries(backend ${Boost_LIBRARIES})
target_link_libraries(backend Porter2_Stemmer)
target_link_libraries(backend PocoJSON)
target_link_libraries(backend PocoNet)
target_link_libraries(backend PocoFoundation)
target_link_libraries(backend pthread)
