#include "sortTitlesHelper.h"
#include <algorithm>
#include <numeric>

void read_into_vector(std::istream& istr, std::vector<std::string>& vec)
{
	std::string line;
	while(std::getline(istr, line))
		vec.push_back(line);
}

void sort_article_files(std::istream& article_titles_file, std::istream& article_dates_file, std::ostream& article_titles_output_file, std::ostream& article_dates_output_file)
{
	std::vector<std::string> article_titles;
	std::vector<std::string> article_dates;
	read_into_vector(article_titles_file, article_titles);
	read_into_vector(article_dates_file, article_dates);

	if(article_titles.size() != article_dates.size())
		throw std::logic_error("Sizes do not match");

	std::vector<std::size_t> positions(article_titles.size(),0);
	std::iota(positions.begin(), positions.end(), 0);
	std::sort(positions.begin(), positions.end(), [&article_titles] (const std::size_t i1, const std::size_t i2) {
			return article_titles[i1] < article_titles[i2];
			});

	for (auto i : positions) {
		article_titles_output_file << article_titles[i] << std::endl;
		article_dates_output_file << article_dates[i] << std::endl;
	}
}


void sort_category_files(std::istream& category_titles_file, std::ostream& category_titles_output_file)
{
	std::vector<std::string> category_titles;
	read_into_vector(category_titles_file, category_titles);

	std::sort(category_titles.begin(), category_titles.end());

	for (auto title : category_titles) 
		category_titles_output_file << title << std::endl;
}
