#pragma once

// boost
#include <boost/graph/graphml.hpp>
#include <boost/graph/graphviz.hpp>

#include "articleGraph.h"

void writeToGraphViz(std::ostream& ostr, const UndirectedArticleGraph& g);

enum Layout { DOT, NEATO, TWOPI, CIRCO, FDP, SFDP, PATCHWORK, OSAGE };

void drawToSvg(std::string filename, const UndirectedArticleGraph& g, const Layout layout);

boost::dynamic_properties generateDynamicProperties(UndirectedArticleGraph& g);

void writeToGraphMl(std::ostream& ostr, UndirectedArticleGraph& g);


void writeMinimizedGraph(std::ofstream& ostr, const UndirectedArticleGraph& g);

UndirectedArticleGraph readMinimizedGraph(std::ifstream& istr);


UndirectedArticleGraph readFromGraphMl(std::istream& istr);
