#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <boost/filesystem.hpp>
#include <iostream>

std::string timingToReadable(std::size_t milliseconds);

std::map<std::string, std::size_t> readPageCountsFile(std::string path);

void printProgress(const std::map<std::string, std::size_t>& pageCounts, const std::string& path, std::size_t count, const std::string article_title);

bool getPosition(const std::vector<std::string>& vec, std::string str, std::size_t& pos);
