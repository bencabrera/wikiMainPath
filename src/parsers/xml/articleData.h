#pragma once

#include <map>
#include <string>

struct ArticleData {
	ArticleData()
	:IsRedirect(false)
	{}

	std::map<std::string, std::string> MetaData;
	std::string Content;	

	bool IsRedirect;
	std::string RedirectTarget;
};
