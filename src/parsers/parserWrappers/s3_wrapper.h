#pragma once

// boost
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>

// xerces
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>

// local files
#include "../xml/wikiDumpHandler.h"
#include "../wikiHandlers/categoryRecursiveHandler.h"
#include "../shared.h"

namespace fs = boost::filesystem;

class S3ParserWrapper 
{
	public:
		typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, boost::no_property, boost::no_property, boost::vecS> Graph;

		S3ParserWrapper(
			fs::path path, 
			const std::map<std::string, std::size_t>& pageCounts, 
			const std::vector<std::string>& cats, 
			Graph& graph,
			VectorMutex<1000>& vecMut
		);

		void operator()(void);

		std::function<void(std::size_t)> ProgressCallback;

	private:
		fs::path _path;
		const std::map<std::string, std::size_t>& _pageCounts;
		const std::vector<std::string>& _categories; 
		Graph& _graph;
		VectorMutex<1000>& _vecMutex;
};


