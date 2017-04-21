
#include "../core/wikiDataCache.h"
#include <boost/filesystem.hpp>

#include "../../libs/shared/cpp/stepTimer.hpp"

#include "serverApplication.h"

int main(int argc, char* argv[])
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
	timer.start_timing_step("read_event_network","Reading in 'event_network'", &std::cout);
	data.event_network();
	timer.stop_timing_step("read_event_network",&std::cout);
	timer.start_timing_step("read_event_indices","Reading in 'event_indices'", &std::cout);
	data.event_indices();
	timer.stop_timing_step("read_event_indices",&std::cout);

	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "IO Timings: " << std::endl;
	timer.print_timings(std::cout);
	std::cout << "---------------------------------------------------" << std::endl;

	Shared::StepTimer timer_server;

	timer_server.start_timing_step("server_running","Server running", &std::cout);
	Server server(data);
	server.run(argc,argv);
	timer_server.stop_timing_step("server_running", &std::cout);

	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "Running Timings: " << std::endl;
	timer_server.print_timings(std::cout);
	std::cout << "---------------------------------------------------" << std::endl;

	return 0;
}
