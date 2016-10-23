// stdlib
#include <iostream>
#include <iomanip>
#include <map>
#include <future>
#include <thread>
#include <fstream>

// boost
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/subgraph.hpp>
#include <boost/graph/copy.hpp>

// local files
#include "fullTextSearch.h"
#include "../parsers/articleNetwork/dateExtractor.h"

#include "../mpaComputation/checkGraphProperties.hpp"
#include "../mpaComputation/edgeWeightGeneration.hpp"
#include "../mpaComputation/mpa_algorithms.hpp"
#include "readDataFromFile.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char* argv[])
{
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "Produce help message.")
		("input-article-folder", po::value<std::string>(), "The folder that should contain articlesWithDates.txt, categories.txt, redirects.txt files.")
		;
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	// display help if --help was specified
	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return 0;
	}

	if(!vm.count("input-article-folder"))
	{
		std::cerr << "Please specify the parameters --input-article-folder." << std::endl;
		return 1;
	}

	const fs::path inputArticleFolder(vm["input-article-folder"].as<std::string>());

	if(!fs::is_directory(inputArticleFolder))
	{
		std::cerr << "Parameter --input-xml-folder is no folder." << std::endl;
		return 1;
	}


	std::vector<std::string> articles;
	std::vector<Date> dates; 
	std::vector<std::string> categories;
	std::vector<std::vector<std::size_t>> category_has_article;

	auto graph = readDataFromFile(inputArticleFolder, articles, dates, categories, category_has_article);



	auto invertedIndex = buildInvertedIndex(categories);

	while(true)
	{
		std::string queryStr;
		std::cout << std::endl << "Please enter your search query: ";
		std::getline(std::cin, queryStr);

		auto docs = query(invertedIndex, queryStr);
		std::vector<std::size_t> docs_vec(docs.begin(), docs.end());

		std::cout << "---------------------------------------------------" << std::endl;
		std::cout << "Query: " << queryStr << std::endl << std::endl;
		std::cout << "Results: " << std::endl;
		for(std::size_t i = 0; i < docs_vec.size(); i++)
			std::cout << "[" << i+1 << "] " << categories[docs_vec[i]] << " (" << category_has_article[docs_vec[i]].size() << " entries)" << std::endl;

		std::cout << "Please choose a category (0 = new search): ";
		std::size_t choice;
		std::cin >> choice;

		std::cin.ignore (std::numeric_limits<std::streamsize>::max(), '\n'); 

		if(choice == 0)
			continue;

		if(choice > docs_vec.size())
		{
			std::cout << "Chosen index was too large." << std::endl;
			continue;
		}

		std::size_t category_idx = docs_vec[choice-1];
		std::cout << "Articles in category: " << std::endl;
		Subgraph sub = graph.create_subgraph();
		for (auto art_idx : category_has_article[category_idx]) 
		{
			std::cout << articles[art_idx] << std::endl;
			boost::add_vertex(art_idx, sub);
		}

		std::cout << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;

		if(MpaComputation::check_if_acyclic(sub))
			std::cout << "Graph is acyclic" << std::endl;
		else
			std::cout << "Graph is NOT acyclic" << std::endl;

		std::vector<std::size_t> localArticleMapping(boost::num_vertices(sub));
		for(std::size_t i = 0; i < boost::num_vertices(sub); i++)
			localArticleMapping[i] = sub.local_to_global(i);

		Graph g;
		boost::copy_graph(sub, g);

		Graph::vertex_descriptor s, t;
		MpaComputation::addSourceAndSinkVertex<Graph>(g, s, t);
		auto weights = MpaComputation::generate_spc_weights(g, s, t);
		auto mpa = MpaComputation::localForward(g, weights, s, t);
		std::cout << "Main path: " << std::endl;
		for (auto e : mpa) {
			std::cout << localArticleMapping[boost::source(e,g)] << " --> " << localArticleMapping[boost::target(e,g)] << std::endl;
			std::cout << articles[localArticleMapping[boost::source(e,g)]] << " --> " << articles[localArticleMapping[boost::target(e,g)]] << std::endl;
		}

	}

	return 0;
}
