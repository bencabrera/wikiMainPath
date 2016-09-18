#include <iostream>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>
#include <iomanip>
#include <map>
#include <future>
#include <thread>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <fstream>

#include <boost/graph/graphml.hpp>
#include <boost/graph/graphviz.hpp>

#include "wikiHandlers/categoryArticleHandler.h"
#include "xml/wikiDumpHandler.h"
#include "wikiHandlers/tagFilterHandler.h"
#include "wikiHandlers/linkExtractionHandler.h"
#include "wikiClassDetection/wikiClasses.h"

typedef std::vector<std::pair<std::string,WikiClassDetection::WikiClassSet>> ArticleList;
typedef std::vector<std::pair<std::string,ArticleNetwork::LinkTargetSet>> ArticleLinksList;

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace std {
	template<typename T>
	T begin(const std::pair<T,T>& eItPair)
	{
		return eItPair.first;
	}
	
	template<typename T>
	T end(const std::pair<T,T>& eItPair)
	{
		return eItPair.second;
	}
}

void writeToGraphViz(std::ostream& ostr, const UndirectedArticleGraph& g)
{
	auto vertex_label_writer = boost::make_label_writer(boost::get(boost::vertex_name, g));
	
	boost::write_graphviz (ostr, g, vertex_label_writer);
}

enum Layout { DOT, NEATO, TWOPI, CIRCO, FDP, SFDP, PATCHWORK, OSAGE };
const std::vector<std::string> layoutCommand = {"dot", "neato", "twopi", "circo", "fdp", "sfdp", "patchwork", "osage"};

void drawToSvg(std::string filename, const UndirectedArticleGraph& g, const Layout layout)
{
	std::ofstream tmp_file("tmp.dot");
	writeToGraphViz (tmp_file, g);
	std::string command = std::string("dot -K") + layoutCommand[layout] + std::string(" -Tsvg tmp.dot -o ")+filename;
	int flag = std::system(command.c_str());
	if(flag != 0)
		throw std::logic_error("Command dot does not exist.");
//	std::remove("tmp.dot");
}

boost::dynamic_properties generateDynamicProperties(UndirectedArticleGraph& g)
{
	boost::dynamic_properties dp;
	auto tmp = boost::get(boost::vertex_name, g);
	dp.property("vLabel", tmp);
	return dp;
}

void writeToGraphMl(std::ostream& ostr, UndirectedArticleGraph& g)
{
	boost::dynamic_properties dp = generateDynamicProperties(g);
	
	boost::write_graphml(ostr, g, dp);
}


void writeMinimizedGraph(std::ofstream& ostr, const UndirectedArticleGraph& g)
{
	ostr << boost::num_vertices(g) << " " << boost::num_edges(g) << std::endl;
	for (auto v : boost::vertices(g)) 
		ostr << boost::get(boost::vertex_name, g, v) << std::endl;	
	
	for (auto e : boost::edges(g)) 
		ostr << boost::source(e,g) << " " << boost::target(e,g) << std::endl;
}

UndirectedArticleGraph readMinimizedGraph(std::ifstream& istr)
{
	UndirectedArticleGraph g;

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
		UndirectedArticleGraph::vertex_descriptor source, target;
		istr >> source >> target;
		boost::add_edge(source, target, g);
	}

	return g;
}


UndirectedArticleGraph readFromGraphMl(std::istream& istr)
{
	UndirectedArticleGraph g;
	boost::dynamic_properties dp = generateDynamicProperties(g);
	
	boost::read_graphml(istr, g, dp);

	return g;
}

UndirectedArticleGraph createGraph(po::variables_map& vm)
{
	if(vm.count("input-graph-minimized"))
	{
		std::ifstream inFile(vm["input-graph-minimized"].as<std::string>());
		return readMinimizedGraph(inFile);
	}

	if(vm.count("input-graph-graphml"))
	{
		std::ifstream inFile(vm["input-graph-graphml"].as<std::string>());
		return readFromGraphMl(inFile);
	}

	return UndirectedArticleGraph();
}

int main(int argc, char* argv[])
{
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "Produce help message.")
		("input-xml-folder", po::value<std::string>(), "The folder that should be scanned for wikidump .xml files.")
		("input-graph-graphml", po::value<std::string>(), "The .graphml file which contains an already extracted graph.")
		("input-graph-minimized", po::value<std::string>(), "The graph file which contains an already extracted graph.")
		("output-graph-graphml", po::value<std::string>(), "The .graphml file which should contain the output graph.")
		("output-graph-minimized", po::value<std::string>(), "The graph file which should contain the output graph.")
		("max-threads", po::value<std::size_t>(), "The maximal number of parallel threads that should be used for parsing.")
	;
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	// display help if --help was specified
	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return 0;
	}

	if(!vm.count("input-xml-folder") || !(vm.count("output-graph-graphml") || vm.count("output-graph-minimized")))
	{
		std::cerr << "Please specify the parameters --input-xml-folder and --output-graph-graphml or --output-graph-minimized." << std::endl;
		return 1;
	}

	const fs::path inputFolder(vm["input-xml-folder"].as<std::string>());
	//const bool inputGraphSpecified = vm.count("input-graph-graphml");
	//std::size_t numMaxThreads = vm.count("max-threads") ? vm["max-threads"].as<std::size_t>() : std::thread::hardware_concurrency();

	if(!fs::is_directory(inputFolder))
	{
		std::cerr << "Parameter --input-xml-folder is no folder." << std::endl;
		return 1;
	}

	try {
		xercesc::XMLPlatformUtils::Initialize();
	}
	catch (const xercesc::XMLException& toCatch) {
		char* message = xercesc::XMLString::transcode(toCatch.getMessage());
		std::cout << "Error during initialization! :\n";
		std::cout << "Exception message is: \n" << message << "\n";
		xercesc::XMLString::release(&message);
		return 1;
	}

	std::vector<fs::path> xmlFileList;
	for(auto dir_it = fs::directory_iterator(inputFolder); dir_it != fs::directory_iterator(); dir_it++)
	{
		if(!fs::is_directory(dir_it->path()))
			xmlFileList.push_back(dir_it->path());
	}

	UndirectedArticleGraph g = createGraph(vm);
	LinkExtractionHandler linkExtractionHandler(g); 

	for (auto el : xmlFileList) {
		std::cout << el << std::endl;

		xercesc::SAX2XMLReader* parser = xercesc::XMLReaderFactory::createXMLReader();
		parser->setFeature(xercesc::XMLUni::fgSAX2CoreValidation, true);
		parser->setFeature(xercesc::XMLUni::fgSAX2CoreNameSpaces, true);   // optional
		parser->setFeature(xercesc::XMLUni::fgXercesSchema , false);   // optional
		
		WikiDumpHandler handler(linkExtractionHandler, true);
		parser->setContentHandler(&handler);
		parser->setErrorHandler(&handler);
		
		try {
			parser->parse(el.c_str());
			delete parser;
		}
		catch (const xercesc::XMLException& toCatch) {
			char* message = xercesc::XMLString::transcode(toCatch.getMessage());
			std::cerr << "Exception message is: \n" << message << "\n";
			xercesc::XMLString::release(&message);
			throw std::logic_error("An error occurred");
		}
		catch (const xercesc::SAXParseException& toCatch) {
			char* message = xercesc::XMLString::transcode(toCatch.getMessage());
			std::cerr << "Exception message is: \n" << message << "\n";
			xercesc::XMLString::release(&message);
			throw std::logic_error("An error occurred");
		}
	}

	std::cout << "Number of nodes: " << boost::num_vertices(linkExtractionHandler.graph()) << std::endl;
	std::cout << "Number of edges: " << boost::num_edges(linkExtractionHandler.graph()) << std::endl;

	drawToSvg("hallo.svg", linkExtractionHandler.graph(), DOT);

	if(vm.count("output-graph-minimized"))
	{
		std::ofstream graphFile(vm["output-graph-minimized"].as<std::string>());
		writeMinimizedGraph(graphFile, linkExtractionHandler.graph());
	}

	if(vm.count("output-graph-graphml"))
	{
		std::ofstream graphFile(vm["output-graph-graphml"].as<std::string>());
		writeToGraphMl(graphFile, linkExtractionHandler.graph());
	}

	try {
		xercesc::XMLPlatformUtils::Terminate();
	}
	catch (const xercesc::XMLException& toCatch) {
		char* message = xercesc::XMLString::transcode(toCatch.getMessage());
		std::cout << "Error during termination! :\n";
		std::cout << "Exception message is: \n" << message << "\n";
		xercesc::XMLString::release(&message);
		return 1;
	}

	std::cout << "HALLO" << std::endl;

	return 0;
}
