#pragma once

#include <mutex>
#include <string>
#include <map>

void print_progress(
	std::mutex& mutex,
	std::map<std::string,std::size_t>& current_counts,
	const std::size_t& total_count,
	const std::chrono::time_point<std::chrono::steady_clock>& start_time,
	std::size_t count, 
	const std::string& path_str, 
	std::string article_title
);
