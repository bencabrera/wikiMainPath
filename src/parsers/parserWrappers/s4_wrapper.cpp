#include "s4_wrapper.h"

S4ParserWrapper::S4ParserWrapper(
		fs::path path, 
		const std::map<std::string, std::size_t>& pageCounts, 
		const std::vector<std::string>& arts, 
		const std::vector<Date>& dates, 
		const std::map<std::string, std::string>& redirs,
		std::vector<boost::container::flat_set<std::size_t>>& adjList,
		VectorMutex<1000>& vecMut	
		)
:_path(path),
	_pageCounts(pageCounts),
	_articles(arts),
	_dates(dates),
	_redirects(redirs),
	_adjList(adjList),
	_vecMutex(vecMut)
{
	ProgressCallback = std::bind(printProgress, _pageCounts, _path, std::placeholders::_1);
}

void S4ParserWrapper::operator()(void)
{
	LinkExtractionHandler artHandler(_articles, _redirects, _adjList, _vecMutex); 
	if(OrderCallback)
		artHandler.OrderCallback = OrderCallback;

	xercesc::SAX2XMLReader* parser = xercesc::XMLReaderFactory::createXMLReader();
	parser->setFeature(xercesc::XMLUni::fgSAX2CoreValidation, true);
	parser->setFeature(xercesc::XMLUni::fgSAX2CoreNameSpaces, true);   // optional
	parser->setFeature(xercesc::XMLUni::fgXercesSchema , false);   // optional

	WikiDumpHandler handler(artHandler, true);
	handler.TitleFilter = [](const std::string& title) {
		return !(
				title.substr(0,5) == "User:" 
				|| title.substr(0,10) == "Wikipedia:" 
				|| title.substr(0,5) == "File:" 
				|| title.substr(0,14) == "Category talk:" 
				|| title.substr(0,9) == "Category:"
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

	parser->parse(_path.c_str());
	delete parser;
}
