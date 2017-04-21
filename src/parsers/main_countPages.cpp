// stdlib
#include <iostream>
#include <iomanip>
#include <map>
#include <future>
#include <thread>
#include <fstream>

// xerces
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>

// boost
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

// local files
#include "../../libs/wiki_xml_dump_xerces/src/examples/countPages/countPagesXercesHandler.hpp"
#include "shared.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

class SingleFileCounter {
	public:
		SingleFileCounter(std::string path)
			:_path(path)
		{}

		std::size_t operator()(void)
		{
			xercesc::SAX2XMLReader* parser = xercesc::XMLReaderFactory::createXMLReader();
			parser->setFeature(xercesc::XMLUni::fgSAX2CoreValidation, true);
			parser->setFeature(xercesc::XMLUni::fgSAX2CoreNameSpaces, true);   // optional
			parser->setFeature(xercesc::XMLUni::fgXercesSchema , false);   // optional

			CountPagesXercesHandler handler;
			parser->setContentHandler(&handler);
			parser->setErrorHandler(&handler);

			parser->parse(_path.c_str());
			delete parser;

			return handler.count();
		}

	private:
		std::string _path;
};




int main(int argc, char* argv[])
{
	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "Produce help message.")
		("input-xml-folder,i", po::value<std::string>(), "The file that should be scanned for <page> tags.")
		("selection-file,s", po::value<std::string>(), "The file that contains a list of all .xml files which should be considered.")
		("count-pages-file,o", po::value<std::string>(), "The file that should be scanned for <page> tags.")

		;
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	// display help if --help was specified
	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return 0;
	}

	if(!vm.count("input-xml-folder"))
	{
		std::cerr << "Please specify the parameter --input-xml-folder." << std::endl;
		return 1;
	}

	if(!vm.count("count-pages-file"))
	{
		std::cerr << "Please specify the parameter --count-pages-file." << std::endl;
		return 1;
	}

	const fs::path inputFolder(vm["input-xml-folder"].as<std::string>());

	if(!fs::is_directory(inputFolder))
	{
		std::cerr << "Folder at --input-xml-folder does not exist." << std::endl;
		return 1;
	}

	// init xerces
	xercesc::XMLPlatformUtils::Initialize();

	// scan for xml files in the input-folder
	std::vector<std::string> paths;
	if(vm.count("selection-file"))
		paths = selected_filename_in_folder(inputFolder, fs::path(vm["selection-file"].as<std::string>()));
	else
		paths = selected_filename_in_folder(inputFolder);
		

	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Found the following files: " << std::endl;
	for (auto f : paths) 
		std::cout << f << std::endl;

	std::vector<std::future<std::size_t>> futures;
	futures.reserve(paths.size());
	for (auto f : paths)
		futures.emplace_back(std::async(std::launch::async, SingleFileCounter(f)));

	std::map<std::string, std::size_t> pageCounts;
	for (std::size_t i = 0; i < futures.size(); i++)
		pageCounts.insert({ fs::path(paths[i]).filename().string(), futures[i].get() });

	xercesc::XMLPlatformUtils::Terminate();

	std::ofstream output_file(vm["count-pages-file"].as<std::string>(), std::ios_base::app);
	for (auto file : pageCounts) 
		output_file << file.first << " " << file.second << std::endl;

	return 0;
}
