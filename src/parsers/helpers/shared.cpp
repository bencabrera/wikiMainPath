#include "shared.h"

#include "../../libs/shared/cpp/cliProgressBar.hpp"
#include <set>
#include <boost/algorithm/string/trim.hpp>

std::string timingToReadable(std::size_t milliseconds)
{
	std::stringstream ss;
	
	auto hours = (milliseconds / (1000*60*60));
	auto mins =  (milliseconds % (1000*60*60)) / (1000*60);
	auto seconds =  ((milliseconds % (1000*60*60)) % (1000*60)) / 1000;
	auto milli =  (((milliseconds % (1000*60*60)) % (1000*60)) % 1000);
	ss << hours << "h " << mins << "m " << seconds << "s " << milli << "ms";

	return ss.str();
}

std::map<std::string, std::size_t> readPageCountsFile(std::string path)
{
	std::ifstream istr(path);
	std::map<std::string, std::size_t> rtn;

	while(!istr.eof())
	{
		std::string filename;
		std::size_t count;
		istr >> filename >> count;

		rtn.insert({ filename, count });
	}

	return rtn;
}

std::vector<std::string> selected_filename_in_folder(boost::filesystem::path input_folder, boost::filesystem::path selection_file_path)
{
	namespace fs = boost::filesystem;

	// read in selection file if possible
	std::set<std::string> selected_file_names;
	if(!selection_file_path.empty() && fs::is_regular_file(selection_file_path))
	{
		std::ifstream selection_file(selection_file_path.string());
		std::string line;
		while(std::getline(selection_file, line))
		{
			boost::trim(line);
			if(line.length() > 0)
				selected_file_names.insert(line);
		}
	}

	std::vector<std::string> xml_file_list;
	for(auto dir_it = fs::directory_iterator(input_folder); dir_it != fs::directory_iterator(); dir_it++)
	{
		std::string filename = dir_it->path().filename().string();
		if(fs::is_regular_file(dir_it->path()) && (selected_file_names.size() == 0 || selected_file_names.count(filename) > 0))
			xml_file_list.push_back(dir_it->path().string());
	}

	return xml_file_list;
}

void printProgress(const std::map<std::string, std::size_t>& pageCounts, const std::string& path_str, std::size_t count, std::string article_title)
{
	boost::filesystem::path path(path_str);
	auto it = pageCounts.find(path.filename().c_str());
	std::cout << path.filename();
	if(!article_title.empty())
		std::cout << " [" << article_title << "]";
	std::cout << ": ";

	if(it != pageCounts.end())
	{
		std::cout << std::endl;
		Shared::cli_progress_bar(count, it->second);
		std::cout << std::endl;
		// std::cout << std::right << count << " / " << it->second << "  [" << ((int)(100*(double)count/it->second)) << " %]" << std::endl;
	}
	else
		std::cout << std::right << count << std::endl;
	std::cout.flush();
}

bool getPosition(const std::vector<std::string>& vec, std::string str, std::size_t& pos)
{
	auto it = std::lower_bound(vec.begin(), vec.end(), str);	
	if(it == vec.end() || *it != str)
		return false;
	else
	{
		pos = it - vec.begin();
		return true;
	}
}
