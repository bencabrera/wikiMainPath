#include "buildArtificialDump.h"

std::string build_artificial_dump(const std::vector<std::string>& titles, const std::vector<std::pair<std::string, std::string>>& links)
{
	static const std::string start = "<mediawiki xmlns=\"http://www.mediawiki.org/xml/export-0.10/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.mediawiki.org/xml/export-0.10/ http://www.mediawiki.org/xml/export-0.10.xsd\" version=\"0.10\" xml:lang=\"en\">";
	static const std::string end = "</mediawiki>";
	static const std::string top = "<page> \
    <title>";
	static const std::string middle = "</title> \
    <revision> \
      <text>";
	static const std::string bottom = "</text> \
    </revision> \
  </page>";

	std::stringstream out;
	out << start;

	for (auto title : titles) {
		out << top << title << middle;
		for (auto link : links) 
			if(link.first == title)
			{
				out << "[[" << link.second << "]]";
			}
		out << bottom;
	}

	out << end;

	return out.str();
}
