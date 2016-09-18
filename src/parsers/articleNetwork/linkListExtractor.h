#pragma once

#include <iostream>
#include <set>
#include <string>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>

namespace ArticleNetwork {
	typedef std::set<std::string> LinkTargetSet;

	class LinkListExtractor {
	public:
		LinkTargetSet operator()(const std::string& content);
	};
}
