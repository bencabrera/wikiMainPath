#include <string>
#include <vector>
#include <map>

std::string timingToReadable(std::size_t milliseconds);

std::map<std::string, std::size_t> readPageCountsFile(std::string path);

bool getPosition(const std::vector<std::string>& vec, std::string str, std::size_t& pos);

std::vector<std::string> read_lines_from_file(std::istream& input_file);
