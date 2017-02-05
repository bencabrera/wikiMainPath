#include <string>
#include <set>
#include <map>
#include <vector>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>

namespace WikiMainPath {

	using InvertedIndex = std::map<std::string, std::set<std::size_t>>;
	using DocumentSet = std::set<std::size_t>;

	std::string stem(std::string str);

	InvertedIndex buildInvertedIndex(const std::vector<std::string>& documents);

	DocumentSet query(const InvertedIndex& invertedIndex, std::string queryStr);

}
