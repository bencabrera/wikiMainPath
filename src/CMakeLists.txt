cmake_minimum_required(VERSION 3.1)
project(src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror")

# Boost stuff
# WARNING: Boost needs to be build using -fPIC option
set(BOOST_ROOT /usr/local/include/boost/)
set(BOOST_LIBRARYDIR /usr/local/lib/)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.54 REQUIRED COMPONENTS filesystem system program_options graph)

include_directories(${Boost_INCLUDE_DIRS})

set(MAIN
	parsers/xml/abstractArticleHandler.h
	parsers/xml/articleData.h
	parsers/xml/wikiDumpHandler.h
	parsers/xml/wikiDumpHandler.cpp
	parsers/wikiHandlers/articleTitlesHandler.h
	parsers/wikiHandlers/articleTitlesHandler.cpp
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
)

set(PHP_EXTENSION_FILES 
	phpExtension/wikiMainPath.cpp
	program/fullTextSearch.h
	program/fullTextSearch.cpp
)

execute_process(COMMAND php-config7.0 --extension-dir OUTPUT_VARIABLE PHP_CONFIG_DIR)
set(PHP_APACHE_INI_DIR /etc/php/7.0/mods-available/)

add_library( Main STATIC ${MAIN} )

add_executable(extractTitlesAndDates parsers/main_extractTitlesAndDates.cpp)
target_link_libraries(extractTitlesAndDates Main)
target_link_libraries(extractTitlesAndDates ${Boost_LIBRARIES})
target_link_libraries(extractTitlesAndDates xerces-c)
target_link_libraries(extractTitlesAndDates pthread)

add_executable(articleNetwork parsers/main_buildArticleNetwork.cpp)
target_link_libraries(articleNetwork Main)
target_link_libraries(articleNetwork ${Boost_LIBRARIES})
target_link_libraries(articleNetwork xerces-c)
target_link_libraries(articleNetwork pthread)

add_executable(prog program/main_programm.cpp)
target_link_libraries(prog ${Boost_LIBRARIES})
target_link_libraries(prog Main)

add_executable(tests parsers/main_tests.cpp)
target_link_libraries(tests Main)
target_link_libraries(tests ${Boost_LIBRARIES})

add_library(wikiMainPath SHARED ${PHP_EXTENSION_FILES})
set_target_properties(wikiMainPath PROPERTIES PREFIX "") #remove the lib in front of shared object file
target_compile_options(wikiMainPath PUBLIC -fPIC -shared)
target_link_libraries(wikiMainPath ${Boost_LIBRARIES})
target_link_libraries(wikiMainPath phpcpp)

add_custom_target(installExtension)
add_custom_command(TARGET installExtension COMMAND cp $<TARGET_FILE:wikiMainPath> ${PHP_CONFIG_DIR}) 
add_custom_command(TARGET installExtension COMMAND phpdismod wikiMainPath) 
add_custom_command(TARGET installExtension COMMAND cp ${CMAKE_SOURCE_DIR}/phpExtension/wikiMainPath.ini ${PHP_APACHE_INI_DIR}) 
add_custom_command(TARGET installExtension COMMAND phpenmod wikiMainPath) 
add_custom_command(TARGET installExtension COMMAND service apache2 restart) 
# todo add apache restart in here
