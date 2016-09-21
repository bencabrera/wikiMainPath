#pragma once

// boost
#include <boost/graph/graphml.hpp>
#include <boost/graph/graphviz.hpp>

#include "articleGraph.h"

void writeToGraphViz(std::ostream& ostr, const DirectedArticleGraph& g);

enum Layout { DOT, NEATO, TWOPI, CIRCO, FDP, SFDP, PATCHWORK, OSAGE };

void drawToSvg(std::string filename, const DirectedArticleGraph& g, const Layout layout);

boost::dynamic_properties generateDynamicProperties(DirectedArticleGraph& g);

void writeToGraphMl(std::ostream& ostr, DirectedArticleGraph& g);


void writeMinimizedGraph(std::ofstream& ostr, const DirectedArticleGraph& g);

DirectedArticleGraph readMinimizedGraph(std::ifstream& istr);


DirectedArticleGraph readFromGraphMl(std::istream& istr);
