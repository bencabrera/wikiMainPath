#include "articleGraphIo.h"

const std::vector<std::string> layoutCommand = {"dot", "neato", "twopi", "circo", "fdp", "sfdp", "patchwork", "osage"};

void writeToGraphViz(std::ostream& ostr, const DirectedArticleGraph& g)
{
	auto vertex_label_writer = boost::make_label_writer(boost::get(boost::vertex_name, g));
	
	boost::write_graphviz (ostr, g, vertex_label_writer);
}

void drawToSvg(std::string filename, const DirectedArticleGraph& g, const Layout layout)
{
	std::ofstream tmp_file("tmp.dot");
	writeToGraphViz (tmp_file, g);
	std::string command = std::string("dot -K") + layoutCommand[layout] + std::string(" -Tsvg tmp.dot -o ")+filename;
	int flag = std::system(command.c_str());
	if(flag != 0)
		throw std::logic_error("Command dot does not exist.");
//	std::remove("tmp.dot");
}

boost::dynamic_properties generateDynamicProperties(DirectedArticleGraph& g)
{
	boost::dynamic_properties dp;
	auto tmp = boost::get(boost::vertex_name, g);
	dp.property("vLabel", tmp);
	return dp;
}

void writeToGraphMl(std::ostream& ostr, DirectedArticleGraph& g)
{
	boost::dynamic_properties dp = generateDynamicProperties(g);
	
	boost::write_graphml(ostr, g, dp);
}


void writeMinimizedGraph(std::ofstream& ostr, const DirectedArticleGraph& g)
{
	ostr << boost::num_vertices(g) << " " << boost::num_edges(g) << std::endl;
	for (auto v : boost::vertices(g)) 
		ostr << boost::get(boost::vertex_name, g, v) << std::endl;	
	
	for (auto e : boost::edges(g)) 
		ostr << boost::source(e,g) << " " << boost::target(e,g) << std::endl;
}

DirectedArticleGraph readMinimizedGraph(std::ifstream& istr)
{
	DirectedArticleGraph g;

	std::size_t numVertices, numEdges;
	istr >> numVertices >> numEdges;

	for(std::size_t i = 0; i < numVertices; i++)
	{
		std::string tmpStr;
		istr >> tmpStr;
		auto v = boost::add_vertex(g);
		boost::put(boost::vertex_name, g, v, tmpStr);
	}

	for(std::size_t i = 0; i < numEdges; i++)
	{
		DirectedArticleGraph::vertex_descriptor source, target;
		istr >> source >> target;
		boost::add_edge(source, target, g);
	}

	return g;
}


DirectedArticleGraph readFromGraphMl(std::istream& istr)
{
	DirectedArticleGraph g;
	boost::dynamic_properties dp = generateDynamicProperties(g);
	
	boost::read_graphml(istr, g, dp);

	return g;
}
