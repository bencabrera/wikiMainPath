#include "../core/wikiDataCache.h"
#include "serverDataCache.h"
#include <boost/filesystem.hpp>
#include <boost/graph/graphviz.hpp>

#include "../../libs/shared/cpp/stepTimer.hpp"

int main(int argc, char** argv)
{
	using namespace WikiMainPath;

	auto data_path = boost::filesystem::path(argv[1]);

	if(!boost::filesystem::is_directory(data_path))
	{
		std::cerr << "Specified folder is non-existent." << std::endl;
		return 1;
	}


	std::cout << "Scanning path " << data_path << " for data files." << std::endl;
	Shared::StepTimer timer;

	timer.start_timing_step("global","Reading in data files", &std::cout);

	WikiDataCache data(data_path.string());
	timer.start_timing_step("read_article_titles","Reading in 'article_titles'", &std::cout);
	data.article_titles();
	timer.stop_timing_step("read_article_titles",&std::cout);
	timer.start_timing_step("read_category_titles","Reading in 'category_titles'", &std::cout);
	data.category_titles();
	timer.stop_timing_step("read_category_titles",&std::cout);
	timer.start_timing_step("read_category_has_article","Reading in 'category_has_article'", &std::cout);
	data.category_has_article();
	timer.stop_timing_step("read_category_has_article",&std::cout);
	timer.start_timing_step("read_article_dates","Reading in 'article_dates'", &std::cout);
	data.article_dates();
	timer.stop_timing_step("read_article_dates",&std::cout);
	timer.start_timing_step("read_article_network","Reading in 'article_network'", &std::cout);
	auto& article_network = data.article_network();
	timer.stop_timing_step("read_article_network",&std::cout);
	// timer.start_timing_step("read_event_network","Reading in 'event_network'", &std::cout);
	// data.event_network();
	// timer.stop_timing_step("read_event_network",&std::cout);
	// timer.start_timing_step("read_event_indices","Reading in 'event_indices'", &std::cout);
	// data.event_indices();
	// timer.stop_timing_step("read_event_indices",&std::cout);

	std::cout << "---------------------------------------------------" << std::endl;
	 std::cout << "IO Timings: " << std::endl;
	timer.print_timings(std::cout);
	std::cout << "---------------------------------------------------" << std::endl;



	Shared::StepTimer timer_server;

	ServerDataCache _server_data_cache(data);

	const std::size_t category_id = 1564179;

	timer_server.start_timing_step("build_event_list", "Build event list", &std::cout);
	const auto& event_list = _server_data_cache.get_event_list(category_id);
	std::cout << event_list.size() << std::endl;
	timer_server.stop_timing_step("build_event_list", &std::cout);

	for (auto event : event_list) {
		std::cout << std::setw(80) << event.Title << " " << to_iso_string(event.Date) << std::endl;	
	}

	timer_server.start_timing_step("build_event_network", "Build event network", &std::cout);
	const auto& subgraph = _server_data_cache.get_event_network(category_id);
	std::cout << boost::num_vertices(subgraph) << std::endl;
	timer_server.stop_timing_step("build_event_network", &std::cout);

	std::ofstream graphviz_file("/home/cabrera/Schreibtisch/test.dot");
	boost::write_graphviz(graphviz_file, subgraph);

	timer_server.start_timing_step("build_positions", "Build positions", &std::cout);
	const auto& positions = _server_data_cache.get_network_positions(category_id);
	std::cout << positions.size() << std::endl;
	std::cout << "positions: " << std::endl;
	for (auto pos : positions) {
		std::cout << pos << std::endl;
	}
	timer_server.stop_timing_step("build_positions", &std::cout);


	std::cout << "Loops in article network" << std::endl;
	
	std::size_t v1 = 0, n_loops = 0, n_multi_edges = 0, previous = 0;
	for (auto neighbors : article_network) 
	{
		previous = 0;
		for (auto v2 : neighbors)
		{
			if(v1 == v2)
			{
				std::cout << v1 << " --> " << v2 << std::endl;
				n_loops++;
			}
			if(v2 == previous)
				n_multi_edges++;
			previous = v2;
		}
		v1++;
	}
	
	std::cout << "In total found " << n_loops << " loops and " << n_multi_edges << " multi edges" << std::endl;

	// timer_server.start_timing_step("build_main_path", "Build main path", &std::cout);
	// const auto& main_path = _server_data_cache.get_global_main_path(category_id);
	// std::cout << main_path.size() << std::endl;
	// timer_server.stop_timing_step("build_main_path", &std::cout);

	timer_server.print_timings(std::cout);



	return 0;
}
