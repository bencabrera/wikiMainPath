#include <string>
#include <set>
#include <map>
#include <vector>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>

typedef std::map<std::string, std::set<std::size_t>> InvertedIndex;
typedef std::set<std::size_t> DocumentSet;

std::string stem(std::string str);

InvertedIndex buildInvertedIndex(const std::vector<std::string>& documents);

DocumentSet query(const InvertedIndex& invertedIndex, std::string queryStr);
