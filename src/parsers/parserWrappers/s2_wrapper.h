#pragma once

// xerces
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>

// boost
#include <boost/filesystem.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/algorithm/string/trim.hpp>

// local files
#include "../xml/wikiDumpHandler.h"
#include "../wikiHandlers/categoryHasArticleHandler.h"
#include "../shared.h"

namespace fs = boost::filesystem;

class S2ParserWrapper 
{
	public:
		S2ParserWrapper(
			fs::path path, 
			const std::map<std::string, std::size_t>& pageCounts, 
			const std::vector<std::string>& arts, 
			const std::vector<std::string>& cats, 
			std::vector<boost::container::flat_set<std::size_t>>& catHasArt, 
			VectorMutex<1000>& vecMut
		);

		void operator()(void);

		std::function<void(std::size_t)> ProgressCallback;
	private:
		fs::path _path;
		const std::map<std::string, std::size_t>& _pageCounts;


		const std::vector<std::string>& _articles;
		const std::vector<std::string>& _categories; 
		std::vector<boost::container::flat_set<std::size_t>>& _categoryHasArticle; 
		VectorMutex<1000>& _vecMutex;
};


