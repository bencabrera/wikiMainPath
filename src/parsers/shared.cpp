#include "shared.h"

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

void printProgress(const std::map<std::string, std::size_t>& pageCounts, const std::string& path_str, std::size_t count, std::string article_title)
{
	boost::filesystem::path path(path_str);
	auto it = pageCounts.find(path.filename().c_str());
	std::cout << path.filename();
	if(!article_title.empty())
	{
		std::cout << " [" << article_title << "]";
	}
	std::cout << ": ";

	if(it != pageCounts.end())
	{
		std::cout << std::right << count << " / " << it->second << "  [" << ((int)(100*(double)count/it->second)) << " %]" << std::endl;
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
