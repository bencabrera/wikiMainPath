#include "s3_wrapper.h"

S3ParserWrapper::S3ParserWrapper(
		fs::path path, 
		const std::map<std::string, std::size_t>& pageCounts, 
		const std::vector<std::string>& cats, 
		Graph& graph,
		VectorMutex<1000>& vecMut
		)
:_path(path),
	_pageCounts(pageCounts),
	_categories(cats),
	_graph(graph),
	_vecMutex(vecMut)
{
	if(boost::num_vertices(_graph) != _categories.size())
		throw std::logic_error("Graph for recursive categories does not have the correct number of vertices.");

	ProgressCallback = std::bind(printProgress, _pageCounts, _path, std::placeholders::_1);
}

void S3ParserWrapper::operator()(void)
{
	CategoryRecursiveHandler artHandler(_categories, _graph, _vecMutex);
	xercesc::SAX2XMLReader* parser = xercesc::XMLReaderFactory::createXMLReader();
	parser->setFeature(xercesc::XMLUni::fgSAX2CoreValidation, true);
	parser->setFeature(xercesc::XMLUni::fgSAX2CoreNameSpaces, true);   // optional
	parser->setFeature(xercesc::XMLUni::fgXercesSchema , false);   // optional

	WikiDumpHandler handler(artHandler, true);
	handler.TitleFilter = [](const std::string& title) {
		return title.substr(0,9) == "Category:";
	};
	handler.ProgressCallback = ProgressCallback;

	parser->setContentHandler(&handler);
	parser->setErrorHandler(&handler);

	parser->parse(this->_path.c_str());
	delete parser;
}
