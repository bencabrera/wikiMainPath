cmake_minimum_required(VERSION 3.1)
project(src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror")

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
	parsers/articleNetwork/articleGraphIo.h
	parsers/articleNetwork/articleGraphIo.cpp
)

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
