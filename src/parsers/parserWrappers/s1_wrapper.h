#pragma once

// boost
#include <boost/filesystem.hpp>

// xerces
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>

// local files
#include "../xml/wikiDumpHandler.h"
#include "../wikiHandlers/articleDatesAndCategoriesHandler.h"
#include "../shared.h"

namespace fs = boost::filesystem;

class S1ParserWrapper 
{
	public:
		S1ParserWrapper(fs::path path, const std::map<std::string, std::size_t>& pageCounts, bool extractOnlyArticlesWithDates = true);

		ArticleDatesAndCategoriesHandler operator()(void);

		std::function<void(std::size_t)> ProgressCallback;
	private:
		fs::path _path;

		const std::map<std::string, std::size_t>& _pageCounts;
		bool _extractOnlyArticlesWithDates;
};


