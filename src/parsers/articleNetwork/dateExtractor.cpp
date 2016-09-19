#include "dateExtractor.h"
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <regex>
#include <string>
#include <iomanip>

bool DateExtractor::operator()(const std::string& articleSyntax, std::tm& dateObj) const
{
	return extractDateFromInfobox(articleSyntax, dateObj);
}



std::tm DateExtractor::deserialize(std::string str)
{
	std::tm rtn;

	std::istringstream ss(str);
	std::string token;

	std::getline(ss, token, '_');
	rtn.tm_year = std::stoi(token);
	std::getline(ss, token, '_');
	rtn.tm_mon = std::stoi(token);
	std::getline(ss, token, '_');
	rtn.tm_mday = std::stoi(token);

	return rtn;
}

std::string DateExtractor::serialize(std::tm date)
{
	std::ostringstream ss;
	ss << date.tm_year << "_" << date.tm_mon << "_" << date.tm_mday;

	return ss.str();
}


bool DateExtractor::extractDateFromInfobox(const std::string& articleSyntax, std::tm& dateObj) const
{
	std::size_t foundPos = articleSyntax.find("{{");
	while(foundPos != std::string::npos)
	{
		std::size_t secondFoundPos = articleSyntax.find("}}", foundPos);
		if(secondFoundPos == std::string::npos)
			break;

		std::string val = articleSyntax.substr(foundPos + 2, secondFoundPos - foundPos - 2);
		boost::trim(val);

		if(val.length() > 7 && boost::to_lower_copy(val.substr(0,7)) == "infobox")
		{
			std::regex infoboxDateStrRegex("[Dd]ate\\s*=\\s*([^\\n]*)");
			std::smatch dateStrMatch;
			if(!std::regex_search(val, dateStrMatch, infoboxDateStrRegex)) 
			{
				foundPos = articleSyntax.find("{{", secondFoundPos);
				continue;
			}

			return extractDateFromDateStr(dateStrMatch[1], dateObj);
		}

		foundPos = articleSyntax.find("{{", secondFoundPos);
	}

	return false;
}

bool DateExtractor::extractDateFromDateStr(std::string dateStr, std::tm& dateObj) const
{
	const std::vector<std::string> parserFormats = {
		"%d %b %Y",
		"%Y"
	};

	for (auto format : parserFormats) {
		std::istringstream ss(dateStr);
		ss >> std::get_time(&dateObj, format.c_str());	
		if(!ss.fail())
			return true;
	}

	return false;
}
