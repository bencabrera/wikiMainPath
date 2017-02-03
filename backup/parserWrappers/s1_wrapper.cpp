#include "s1_wrapper.h"

S1ParserWrapper::S1ParserWrapper(fs::path path, const std::map<std::string, std::size_t>& pageCounts, bool extractOnlyArticlesWithDates)
:_path(path),
_pageCounts(pageCounts),
_extractOnlyArticlesWithDates(extractOnlyArticlesWithDates)
{
	ProgressCallback = std::bind(printProgress, _pageCounts, _path, std::placeholders::_1);
}

ArticleDatesAndCategoriesHandler S1ParserWrapper::operator()(void)
{
	ArticleDatesAndCategoriesHandler artHandler; 
	artHandler.ExtractOnlyArticlesWithDates = _extractOnlyArticlesWithDates;

	xercesc::SAX2XMLReader* parser = xercesc::XMLReaderFactory::createXMLReader();
	parser->setFeature(xercesc::XMLUni::fgSAX2CoreValidation, true);
	parser->setFeature(xercesc::XMLUni::fgSAX2CoreNameSpaces, true);   // optional
	parser->setFeature(xercesc::XMLUni::fgXercesSchema , false);   // optional

	// set up call back handlers
	WikiDumpHandler handler(artHandler, true);
	handler.TitleFilter = [](const std::string& title) {
		return !(
				title.substr(0,5) == "User:" 
				|| title.substr(0,10) == "Wikipedia:" 
				|| title.substr(0,5) == "File:" 
				|| title.substr(0,14) == "Category talk:" 
				|| title.substr(0,14) == "Template talk:"
				|| title.substr(0,9) == "Template:"
				|| title.substr(0,10) == "User talk:"
				|| title.substr(0,10) == "File talk:"
				|| title.substr(0,15) == "Wikipedia talk:"
				);
	};
	handler.ProgressCallback = ProgressCallback;

	parser->setContentHandler(&handler);
	parser->setErrorHandler(&handler);

	// run parser
	parser->parse(this->_path.c_str());
	delete parser;

	return artHandler;
}

