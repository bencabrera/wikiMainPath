cmake_minimum_required(VERSION 3.1)
project(src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror")

# Boost stuff
# WARNING: Boost needs to be build using -fPIC option
set(BOOST_ROOT /usr/local/include/boost/)
set(BOOST_LIBRARYDIR /usr/local/lib/)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.54 REQUIRED COMPONENTS filesystem system program_options graph regex)

include_directories(${Boost_INCLUDE_DIRS})

set(MAIN
	parsers/xml/abstractArticleHandler.h
	parsers/xml/articleData.h
	parsers/xml/wikiDumpHandler.h
	parsers/xml/wikiDumpHandler.cpp
	parsers/xml/countPagesHandler.h
	parsers/xml/countPagesHandler.cpp
	parsers/wikiHandlers/articleTitlesHandler.h
	parsers/wikiHandlers/articleTitlesHandler.cpp
	parsers/wikiHandlers/articleDatesAndCategoriesHandler.h
	parsers/wikiHandlers/articleDatesAndCategoriesHandler.cpp
	parsers/wikiHandlers/categoryHasArticleHandler.h
	parsers/wikiHandlers/categoryHasArticleHandler.cpp
	parsers/wikiHandlers/categoryRecursiveHandler.h
	parsers/wikiHandlers/categoryRecursiveHandler.cpp
	parsers/wikiHandlers/linkExtractionHandler.h
	parsers/wikiHandlers/linkExtractionHandler.cpp
	parsers/articleNetwork/linkListExtractor.h
	parsers/articleNetwork/linkListExtractor.cpp
	parsers/articleNetwork/dateExtractor.h
	parsers/articleNetwork/dateExtractor.cpp
	parsers/articleNetwork/date.h
	parsers/articleNetwork/date.cpp
	parsers/articleNetwork/articleGraphIo.h
	parsers/articleNetwork/articleGraphIo.cpp
	program/fullTextSearch.h
	program/fullTextSearch.cpp
	parsers/shared.h
	parsers/shared.cpp
)

set(PHP_EXTENSION_FILES 
	phpExtension/wikiMainPath.cpp
	program/fullTextSearch.h
	program/fullTextSearch.cpp
	parsers/articleNetwork/dateExtractor.h
	parsers/articleNetwork/dateExtractor.cpp
	parsers/articleNetwork/date.h
	parsers/articleNetwork/date.cpp
	parsers/shared.h
	parsers/shared.cpp
)

set(PORTER_2_STEMMER_FILES
	program/porter2_stemmer/porter2_stemmer.h
	program/porter2_stemmer/porter2_stemmer.cpp
	program/porter2_stemmer/util/string_view.h
	program/porter2_stemmer/util/hash.h
)


execute_process(COMMAND php-config7.0 --extension-dir OUTPUT_VARIABLE PHP_CONFIG_DIR)
set(PHP_APACHE_INI_DIR /etc/php/7.0/mods-available/)

add_library( Main STATIC ${MAIN} )

add_library( Porter2_Stemmer STATIC ${PORTER_2_STEMMER_FILES} )
target_compile_options(Porter2_Stemmer PUBLIC -fPIC -shared)

add_executable(testVectorMutex parsers/main_testVectorMutex.cpp)
target_link_libraries(testVectorMutex ${Boost_LIBRARIES})
target_link_libraries(testVectorMutex pthread)

add_executable(testVectorMutex2 parsers/main_testVectorMutex2.cpp)
target_link_libraries(testVectorMutex2 ${Boost_LIBRARIES})
target_link_libraries(testVectorMutex2 pthread)

add_executable(s1_articlesAndCategories parsers/main_s1_articlesAndCategories.cpp)
target_link_libraries(s1_articlesAndCategories Main)
target_link_libraries(s1_articlesAndCategories ${Boost_LIBRARIES})
target_link_libraries(s1_articlesAndCategories xerces-c)
target_link_libraries(s1_articlesAndCategories pthread)

add_executable(s2_categoryHasArticles parsers/main_s2_categoryHasArticles.cpp)
target_link_libraries(s2_categoryHasArticles Main)
target_link_libraries(s2_categoryHasArticles ${Boost_LIBRARIES})
target_link_libraries(s2_categoryHasArticles xerces-c)
target_link_libraries(s2_categoryHasArticles pthread)

add_executable(s3_recursiveCategories parsers/main_s3_recursiveCategories.cpp)
target_link_libraries(s3_recursiveCategories Main)
target_link_libraries(s3_recursiveCategories ${Boost_LIBRARIES})
target_link_libraries(s3_recursiveCategories xerces-c)
target_link_libraries(s3_recursiveCategories pthread)

add_executable(s4_articleNetwork parsers/main_s4_articleNetwork.cpp)
target_link_libraries(s4_articleNetwork Main)
target_link_libraries(s4_articleNetwork ${Boost_LIBRARIES})
target_link_libraries(s4_articleNetwork xerces-c)
target_link_libraries(s4_articleNetwork pthread)

add_executable(countPages parsers/main_countPages.cpp)
target_link_libraries(countPages Main)
target_link_libraries(countPages ${Boost_LIBRARIES})
target_link_libraries(countPages xerces-c)
target_link_libraries(countPages pthread)

add_executable(prog program/main_programm.cpp)
target_link_libraries(prog ${Boost_LIBRARIES})
target_link_libraries(prog Main)
target_link_libraries(prog Porter2_Stemmer)

add_executable(prog2 program/main_programm2.cpp)
target_link_libraries(prog2 ${Boost_LIBRARIES})
target_link_libraries(prog2 Main)
target_link_libraries(prog2 Porter2_Stemmer)



add_executable(parserTests parsers/tests/main_tests.cpp)
target_link_libraries(parserTests Main)
target_link_libraries(parserTests ${Boost_LIBRARIES})

add_library(wikiMainPath SHARED ${PHP_EXTENSION_FILES})
set_target_properties(wikiMainPath PROPERTIES PREFIX "") #remove the lib in front of shared object file
target_compile_options(wikiMainPath PUBLIC -fPIC -shared)
target_link_libraries(wikiMainPath ${Boost_LIBRARIES})
target_link_libraries(wikiMainPath Porter2_Stemmer)
target_link_libraries(wikiMainPath phpcpp)

add_custom_target(installExtension)
add_custom_command(TARGET installExtension COMMAND cp $<TARGET_FILE:wikiMainPath> ${PHP_CONFIG_DIR}) 
add_custom_command(TARGET installExtension COMMAND phpdismod wikiMainPath) 
add_custom_command(TARGET installExtension COMMAND cp ${CMAKE_SOURCE_DIR}/phpExtension/wikiMainPath.ini ${PHP_APACHE_INI_DIR}) 
add_custom_command(TARGET installExtension COMMAND phpenmod wikiMainPath) 
add_custom_command(TARGET installExtension COMMAND service apache2 restart) 
# todo add apache restart in here
