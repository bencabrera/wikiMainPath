#include "../core/wikiDataCache.h"
#include "serverDataCache.h"
#include <boost/filesystem.hpp>

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
	auto& article_titles = data.article_titles();
	timer.stop_timing_step("read_article_titles",&std::cout);
	timer.start_timing_step("read_article_dates","Reading in 'article_dates'", &std::cout);
	auto& article_dates = data.article_dates();
	timer.stop_timing_step("read_article_dates",&std::cout);
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


	for (std::size_t i_article = 0; i_article < article_dates.size(); i_article++) {
		auto& dates = article_dates[i_article];
		for (auto& date : dates) {
			if(date.IsRange)
				std::cout << "[" << i_article << "] " << article_titles[i_article] << ": " << Date::serialize(date) << std::endl;
		}
	}





	return 0;
}
