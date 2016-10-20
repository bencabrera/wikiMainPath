#pragma once 

// xerces
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>

// boost
#include <boost/algorithm/string/split.hpp>
#include <boost/container/flat_set.hpp>

// local files
#include "../wikiHandlers/linkExtractionHandler.h"
#include "../xml/wikiDumpHandler.h"
#include "../articleNetwork/dateExtractor.h"
#include "../shared.h"

namespace fs = boost::filesystem;

class S4ParserWrapper 
{
	public:
		S4ParserWrapper(
			fs::path path, 
			const std::map<std::string, std::size_t>& pageCounts, 
			const std::vector<std::string>& arts, 
			const std::vector<Date>& dates, 
			const std::map<std::string, std::string>& redirs,
			std::vector<boost::container::flat_set<std::size_t>>& adjList,
			VectorMutex<1000>& vecMut	
		);

		void operator()(void);

		std::function<void(std::size_t)> ProgressCallback;
	private:
		fs::path _path;
		const std::map<std::string, std::size_t>& _pageCounts;

		const std::vector<std::string>& _articles;
		const std::vector<Date>& _dates; 
		const std::map<std::string, std::string>& _redirects;
		std::vector<boost::container::flat_set<std::size_t>>& _adjList;
		VectorMutex<1000>& _vecMutex;
};


