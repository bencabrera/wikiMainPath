#include "shared.h"

#include <set>
#include <sstream>
#include <fstream>
#include <chrono>
#include <mutex>
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

	std::string line;
	while(std::getline(istr, line))
	{
		std::string filename;
		std::size_t count;

		boost::trim(line);

		if(line.empty())
			continue;

		std::stringstream ss(line);
		ss >> filename >> count;

		rtn.insert({ filename, count });
	}

	return rtn;
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

std::vector<std::string> read_lines_from_file(std::istream& input)
{
	std::vector<std::string> lines;
	std::string line;
	while(std::getline(input,line))
	{
		boost::trim(line);
		if(!line.empty() && line.front() != '#')
			lines.push_back(line);
	}

	return lines;
}

