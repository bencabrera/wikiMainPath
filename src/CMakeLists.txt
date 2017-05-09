# =====================================================================
# General setup
# =====================================================================
cmake_minimum_required(VERSION 3.1)
project(WikiMainPath)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

include_directories(../libs/eigen)

# --- Boost library setup ---
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.54 REQUIRED COMPONENTS filesystem system program_options graph regex unit_test_framework)



# =====================================================================
# Core
# =====================================================================

# --- Core library ---
set(CORE_FILES
	core/date.cpp
	core/wikiDataCache.cpp
	core/dateDifference.cpp
	core/buildOneCategoryRecursively.cpp
)

add_library(CoreLib STATIC ${CORE_FILES})



# =====================================================================
# Parsers
# =====================================================================

# --- parser static library ---

set(PARSER_FILES
	# handlers
	parsers/wikiArticleHandlers/allLinksArticleHandler.cpp
	parsers/wikiArticleHandlers/articleDatesAndCategoriesHandler.cpp

	# date extraction
	parsers/date/articleDateExtraction.cpp
	parsers/date/infoboxDateExtraction.cpp
	parsers/date/stringDateExtraction.cpp

	# date grammars
	parsers/date/grammars/dateStringGrammar.cpp
	parsers/date/grammars/plainYearGrammar.cpp
	parsers/date/grammars/wikiDateTemplateGrammar.cpp
	parsers/date/grammars/wikiFuzzyDateTemplateGrammar.cpp
	parsers/date/grammars/infoboxKeyValueGrammar.cpp

	parsers/helpers/shared.h
	parsers/helpers/shared.cpp
	parsers/helpers/sortTitlesHelper.cpp

	parsers/helpers/buildCategoryHirachyRecursively.cpp
	parsers/helpers/cycleDetectorVisitor.cpp
	parsers/helpers/removeCyclesInCategoryHirachy.cpp
)

add_library(ParserLib STATIC ${PARSER_FILES})



# --- parser executables ---

add_executable(count_pages parsers/main_countPages.cpp)
target_link_libraries(count_pages ParserLib)
target_link_libraries(count_pages ${Boost_LIBRARIES})
target_link_libraries(count_pages xerces-c)
target_link_libraries(count_pages pthread)

add_executable(s1_articlesAndCategories parsers/main_s1_articlesAndCategories.cpp)
target_link_libraries(s1_articlesAndCategories CoreLib)
target_link_libraries(s1_articlesAndCategories ParserLib)
target_link_libraries(s1_articlesAndCategories ${Boost_LIBRARIES})
target_link_libraries(s1_articlesAndCategories xerces-c)
target_link_libraries(s1_articlesAndCategories pthread)

add_executable(s2_sortArticlesAndCategories parsers/main_s2_sortArticleAndCategories.cpp)
target_link_libraries(s2_sortArticlesAndCategories CoreLib)
target_link_libraries(s2_sortArticlesAndCategories ParserLib)
target_link_libraries(s2_sortArticlesAndCategories ${Boost_LIBRARIES})
target_link_libraries(s2_sortArticlesAndCategories xerces-c)
target_link_libraries(s2_sortArticlesAndCategories pthread)

add_executable(s3_parseAllLinks parsers/main_s3_parseAllLinks.cpp)
target_link_libraries(s3_parseAllLinks ${Boost_LIBRARIES})
target_link_libraries(s3_parseAllLinks CoreLib)
target_link_libraries(s3_parseAllLinks ParserLib)
target_link_libraries(s3_parseAllLinks xerces-c)
target_link_libraries(s3_parseAllLinks pthread)

add_executable(s4_manual parsers/main_s4_manualRecursiveCategories.cpp)
target_link_libraries(s4_manual ${Boost_LIBRARIES})
target_link_libraries(s4_manual CoreLib)
target_link_libraries(s4_manual ParserLib)
target_link_libraries(s4_manual xerces-c)
target_link_libraries(s4_manual pthread)
target_link_libraries(s4_manual ${Boost_LIBRARIES})


# --- parser tests ---

set(PARSER_TEST_FILES
	parsers/tests/articlesAndCategories/articleNetworkTests.cpp
	parsers/tests/articlesAndCategories/categoryTitlesTests.cpp
	parsers/tests/articlesAndCategories/categoryHasArticleTests.cpp
	parsers/tests/articlesAndCategories/articleTitlesTests.cpp
	parsers/tests/articlesAndCategories/cycleDectionTests.cpp
	parsers/tests/date/dateModelTests.cpp
	parsers/tests/date/infoboxDateExtractionTests.cpp
	parsers/tests/date/articleDateExtractionTests.cpp
	parsers/tests/date/dateStringGrammarTests.cpp
	parsers/tests/date/plainYearGrammarTests.cpp
	parsers/tests/date/wikiDateTemplateGrammarTests.cpp
	parsers/tests/date/wikiFuzzyDateTemplateGrammarTests.cpp
	parsers/tests/helpers/buildArtificialDump.cpp
	parsers/tests/main_parserTests.cpp
)

add_executable(parser_tests ${PARSER_TEST_FILES})
target_link_libraries(parser_tests ${Boost_LIBRARIES})
target_link_libraries(parser_tests CoreLib)
target_link_libraries(parser_tests ParserLib)
target_link_libraries(parser_tests xerces-c)
target_link_libraries(parser_tests ${Boost_LIBRARIES})
target_link_libraries(parser_tests ParserLib)
target_link_libraries(parser_tests CoreLib)





# =====================================================================
# Webapp Server Backend
# =====================================================================

set(BACKEND_FILES
	backend/serverDataCache.cpp
	backend/routingHandlerFactory.cpp
	backend/fullTextSearch.cpp
	backend/serverApplication.cpp
	backend/filters.cpp
	# backend/serverData.cpp
	# backend/httpRequestHandlers/mpaHandler.cpp
	backend/httpRequestHandlers/categorySearchHandler.cpp
	backend/httpRequestHandlers/notFoundHandler.cpp
	backend/httpRequestHandlers/articleListHandler.cpp
	backend/httpRequestHandlers/eventNetworkHandler.cpp
	# backend/httpRequestHandlers/articleNetworkHandler.cpp
	backend/graphDrawing/randomGraphDrawing.cpp
	backend/graphDrawing/averagedPrecessorGraphDrawing.cpp
	backend/graphDrawing/forceDirectedGraphDrawing.cpp
	backend/graphDrawing/forceDirectedFunctors.cpp
)

set(PORTER_2_STEMMER_FILES
	../libs/porter2_stemmer/porter2_stemmer.h
	../libs/porter2_stemmer/porter2_stemmer.cpp
	../libs/porter2_stemmer/util/string_view.h
	../libs/porter2_stemmer/util/hash.h
)

add_library(BackendLib STATIC ${BACKEND_FILES})
add_library(Porter2_Stemmer STATIC ${PORTER_2_STEMMER_FILES})

add_executable(backend backend/main_backend.cpp)
target_link_libraries(backend BackendLib)
target_link_libraries(backend CoreLib)
target_link_libraries(backend ${Boost_LIBRARIES})
target_link_libraries(backend Porter2_Stemmer)
target_link_libraries(backend PocoFoundation)
target_link_libraries(backend PocoUtil)
target_link_libraries(backend PocoNet)
target_link_libraries(backend PocoJSON)
target_link_libraries(backend PocoXML)
target_link_libraries(backend pthread)

add_executable(backend_test backend/main_backend_test.cpp)
target_link_libraries(backend_test BackendLib)
target_link_libraries(backend_test CoreLib)
target_link_libraries(backend_test ${Boost_LIBRARIES})
target_link_libraries(backend_test PocoUtil)
target_link_libraries(backend_test PocoNet)
target_link_libraries(backend_test PocoJSON)
target_link_libraries(backend_test PocoXML)
target_link_libraries(backend_test PocoFoundation)
target_link_libraries(backend_test pthread)



# --- backend tests ---

set(BACKEND_TEST_FILES
	backend/tests/forceDirectedGraphDrawingTests.cpp
	backend/tests/main_backendTests.cpp
)

add_executable(backend_tests ${BACKEND_TEST_FILES})
target_link_libraries(backend_tests ${Boost_LIBRARIES})
target_link_libraries(backend_tests BackendLib)
target_link_libraries(backend_tests CoreLib)
target_link_libraries(backend_tests ${Boost_LIBRARIES})
target_link_libraries(backend_tests PocoUtil)
target_link_libraries(backend_tests PocoNet)
target_link_libraries(backend_tests PocoJSON)
target_link_libraries(backend_tests PocoXML)
target_link_libraries(backend_tests PocoFoundation)
target_link_libraries(backend_tests pthread)





add_executable(big_int_test main_big_int_test.cpp)
target_link_libraries(parser_tests ${Boost_LIBRARIES})
