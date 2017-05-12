#include "../core/wikiDataCache.h"
#include "serverDataCache.h"
#include "filters.h"
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
	const auto& category_titles = data.category_titles();
	timer.stop_timing_step("read_category_titles",&std::cout);
	timer.start_timing_step("read_category_has_article","Reading in 'category_has_article'", &std::cout);
	data.category_has_article();
	timer.stop_timing_step("read_category_has_article",&std::cout);
	timer.start_timing_step("read_article_dates","Reading in 'article_dates'", &std::cout);
	data.article_dates();
	timer.stop_timing_step("read_article_dates",&std::cout);
	timer.start_timing_step("read_article_network","Reading in 'article_network'", &std::cout);
	data.article_network();
	timer.stop_timing_step("read_article_network",&std::cout);
	timer.start_timing_step("read_category_hirachy_graph","Reading in 'category_hirachy_graph'", &std::cout);
	data.category_hirachy_graph();
	timer.stop_timing_step("read_category_hirachy_graph",&std::cout);

	std::cout << "---------------------------------------------------" << std::endl;
	 std::cout << "IO Timings: " << std::endl;
	timer.print_timings(std::cout);
	std::cout << "---------------------------------------------------" << std::endl;



	Shared::StepTimer timer_server;

	ServerDataCache _server_data_cache(data);

	// _server_data_cache.article_filters.push_back(articles_without_certain_dates({ "Birth", "Death", "Released", "Recorded", "First aired", "Last aired", "Term started", "Term ended" }));
	// _server_data_cache.event_filters.push_back(events_in_date_range(create_date_range(1750,1,1,1820,1,1)));

	// const std::size_t category_id = 722580; // friendly fire incidents
	// const std::size_t category_id = 719007; // french revolution
	// const std::size_t category_id = 1564179; // world war II
	// const std::size_t category_id = 1409141; // thirty years war
	// const std::size_t category_id = 719009; // french revolution films 
	const std::size_t category_id = 1033233; // norman conquest


	std::cout << "CATEGORY: " << category_titles[category_id] << std::endl;

	RequestParameters request_parameters{ true, 1600, 1700, RequestParameters::LOCAL, 0.0, true, true, "" };

	std::ofstream network_file("/home/cabrera/Schreibtisch/network.txt");
	_server_data_cache.export_event_network_to_file(network_file, category_id, request_parameters);

	// timer_server.start_timing_step("build_article_list", "Build article list", &std::cout);
	const auto& article_list = _server_data_cache.get_article_list(category_id, request_parameters);
	std::cout << article_list.size() << std::endl;
	// timer_server.stop_timing_step("build_article_list", &std::cout);

	// timer_server.start_timing_step("build_event_list", "Build event list", &std::cout);
	const auto& event_list = _server_data_cache.get_event_list(category_id, request_parameters);
	std::cout << event_list.size() << std::endl;
	// timer_server.stop_timing_step("build_event_list", &std::cout);

	// for (auto event : event_list) {
		// std::cout << std::setw(80) << event.Title << " " << to_iso_string(event.Date) << std::endl;	
	// }

	// timer_server.start_timing_step("build_event_network", "Build event network", &std::cout);
	const auto& event_network = _server_data_cache.get_event_network(category_id, request_parameters);
	std::cout << boost::num_vertices(event_network) << std::endl;
	// timer_server.stop_timing_step("build_event_network", &std::cout);

	// // std::ofstream graphviz_file("/home/cabrera/Schreibtisch/test.dot");
	// // boost::write_graphviz(graphviz_file, event_network);

	// timer_server.start_timing_step("build_positions", "Build positions", &std::cout);
	_server_data_cache.get_network_positions(category_id, request_parameters);
	// // std::cout << positions.size() << std::endl;
	// // std::cout << "positions: " << std::endl;
	// // for (auto pos : positions) {
		// // std::cout << pos << std::endl;
	// // }
	// timer_server.stop_timing_step("build_positions", &std::cout);


	// timer_server.start_timing_step("build_main_path", "Build main path", &std::cout);
	const auto& main_path = _server_data_cache.get_global_main_path(category_id, request_parameters);
	std::cout << main_path.size() << std::endl;
	// timer_server.stop_timing_step("build_main_path", &std::cout);

	timer_server.print_timings(std::cout);



	return 0;
}
