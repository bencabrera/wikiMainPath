// stdlib
#include <iostream>
#include <iomanip>
#include <map>
#include <future>
#include <thread>
#include <fstream>

// boost
#include <boost/container/flat_set.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

// local files
#include "../core/date.h"
#include "../core/wikiDataCache.h"

// shared library
#include "../../libs/shared/cpp/stepTimer.hpp"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace WikiMainPath;

int main(int argc, char* argv[])
{
	// setup timings stuff
	Shared::StepTimer timer;
	timer.start_timing_step("global","Total");

	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "Produce help message.")
		("output-folder", po::value<std::string>(), "The folder in which the results (articlesWithDates.txt, categories.txt, redirects.txt) should be stored.")
		;
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	// display help if --help was specified
	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return 0;
	}

	const fs::path outputFolder(vm["output-folder"].as<std::string>());

	if(!vm.count("output-folder"))
	{
		std::cerr << "Please specify output folder using --output-folder." << std::endl;
		return 1;
	}

	timer.start_timing_step("reading", "Reading in already parsed files", &std::cout);
	WikiDataCache wiki_data_cache(outputFolder.string());
	const auto& dates = wiki_data_cache.article_dates();
	const auto& article_network = wiki_data_cache.article_network();
	timer.stop_timing_step("reading");

	timer.start_timing_step("compute_event_indices", "Computing event indices", &std::cout);
	std::size_t current_event_idx = 0;
	std::vector<std::size_t> event_indices;
	event_indices.reserve(dates.size());
	for (auto d : dates) {
		event_indices.push_back(current_event_idx);
		current_event_idx += d.size();
	}
	timer.stop_timing_step("compute_event_indices");

	timer.start_timing_step("write_event_indices", "Writing event indices", &std::cout);
	wiki_data_cache.write_event_indices(event_indices);
	timer.stop_timing_step("write_event_indices");
	

	timer.start_timing_step("compute_event_network", "Compute event network", &std::cout);
	std::vector<boost::container::flat_set<std::size_t>> event_adj_list(event_indices.back()+dates.back().size());
	for (auto v1 : boost::make_iterator_range(boost::vertices(article_network))) 
	{
		auto& dates1 = dates[v1];

		// for every connection in article network connect all the events stemming from dates with each other
		for (auto e : boost::make_iterator_range(boost::out_edges(v1,article_network))) {
			auto v2 = boost::target(e, article_network);	
			auto& dates2 = dates[v2];
			auto start_idx_1 = event_indices[v1];
			auto start_idx_2 = event_indices[v2];

			for(std::size_t i_date_1 = 0; i_date_1 < dates1.size(); i_date_1++) 
				for(std::size_t i_date_2 = 0; i_date_2 < dates2.size(); i_date_2++)
				{
					if(dates1[i_date_1] < dates2[i_date_2])
						event_adj_list[start_idx_1+i_date_1].insert(start_idx_2+i_date_2);	
					else
						event_adj_list[start_idx_2+i_date_2].insert(start_idx_1+i_date_1);	
				}	
		}	

		// connect all inner article events
		auto start_idx_v = event_indices[v1];
		for(std::size_t i_date = 0; i_date < dates1.size(); i_date++) 
			for(std::size_t i_date_to = 0; i_date_to < dates1.size(); i_date_to++) 
				if(i_date != i_date_to && dates1[i_date] < dates1[i_date_to])
					event_adj_list[start_idx_v+i_date].insert(start_idx_v+i_date_to);	
			
	}
	timer.stop_timing_step("compute_event_network");

	timer.start_timing_step("write_event_network", "Write event network", &std::cout);
	wiki_data_cache.write_event_network(event_adj_list);
	timer.stop_timing_step("write_event_network");

	timer.stop_timing_step("global");
	
	// output timings
	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << "Timings: " << std::endl << std::endl;
	timer.print_timings(std::cout);

	std::cout << "-----------------------------------------------------------------------" << std::endl;



	return 0;
}
