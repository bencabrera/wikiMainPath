#include "printProgress.h"

#include <sstream>
#include <iomanip>
#include <iostream>
#include "../../../libs/shared/cpp/cliProgressBar.hpp"

void print_progress(
	std::mutex& mutex,
	std::map<std::string,std::size_t>& current_counts,
	const std::size_t& total_count,
	const std::chrono::time_point<std::chrono::steady_clock>& start_time,
	std::size_t count, 
	const std::string& path_str, 
	std::string article_title
)
{
	std::lock_guard<std::mutex> lock(mutex);

	current_counts.at(path_str) = count;
	
	std::size_t total_current_count = 0;
	for(const auto& p : current_counts)
		total_current_count += p.second;

	auto end_time = std::chrono::steady_clock::now();
	auto milliseconds_passed = std::chrono::duration<double, std::milli>(end_time-start_time).count();
	auto seconds_passed =  milliseconds_passed / 1000;
	std::size_t seconds_to_go = ((seconds_passed * total_count) / total_current_count) - seconds_passed;

	auto hours = (seconds_to_go / (60*60));
	auto mins =  (seconds_to_go % (60*60)) / (60);
	auto seconds =  ((seconds_to_go % (60*60)) % (60));

	std::stringstream ss;
	ss << " --- " << hours << "h " << std::setw(2) << mins << "m " << std::setw(2) << seconds << "s " << "left [" << article_title << "]";

	std::cout << "                                                                                                                                                                                                                                     " << std::endl;
	Shared::cli_progress_bar(total_current_count, total_count, ss.str());
}

