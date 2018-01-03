// stdlib
#include <iostream>
#include <iomanip>
#include <map>
#include <future>
#include <thread>
#include <fstream>
#include <functional>

// boost
#include <boost/filesystem.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/algorithm/string/trim.hpp>

// local files
#include "helpers/shared.h"
#include "../core/wikiDataCache.h"
#include "helpers/removeCyclesInCategoryHirachy.h"

// shared library
#include "../../libs/shared/cpp/stepTimer.hpp"

namespace fs = boost::filesystem;

using namespace WikiMainPath;

int main(int argc, char* argv[])
{
	// setup timings stuff
	Shared::StepTimer timer;
	timer.start_timing_step("global","Total");

	const fs::path outputFolder(argv[1]);

	timer.start_timing_step("reading", "Reading in already parsed files... ", &std::cout);
	WikiDataCache wiki_data_cache(outputFolder.string());
	auto category_has_article = wiki_data_cache.category_has_article_set();
	auto category_hirachy_graph = wiki_data_cache.category_hirachy_graph();
	timer.stop_timing_step("reading", &std::cout);

	// computing recursive category_has_article
	timer.start_timing_step("remove_cycles", "Removing cycles in category_hirachy_graph", &std::cout);
	remove_cycles_in_category_hirachy(category_hirachy_graph);	
	timer.stop_timing_step("remove_cycles", &std::cout);

	// write final category_has_article file
	timer.start_timing_step("final_cha", "Writing final category_has_article", &std::cout);
	wiki_data_cache.write_category_has_article(category_has_article);
	timer.stop_timing_step("final_cha", &std::cout);


	timer.stop_timing_step("global");

	// output timings
	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Timings: " << std::endl << std::endl;
	timer.print_timings(std::cout);

	std::cout << "-----------------------------------------------------------------------" << std::endl;



	return 0;
}
