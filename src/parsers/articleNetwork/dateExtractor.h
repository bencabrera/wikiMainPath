#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>

class DateExtractor {
public:
	bool operator()(const std::string& articleSyntax, std::tm& dateObj) const;

	static std::tm deserialize(std::string str);
	static std::string serialize(std::tm str);
private:
	bool extractDateFromInfobox(const std::string& text, std::tm& dateObj) const;
	bool extractDateFromDateStr(std::string dateStr, std::tm& dateObj) const;
};
