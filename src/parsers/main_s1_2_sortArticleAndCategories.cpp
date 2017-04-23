#include <fstream>
#include <algorithm>
#include <numeric>
#include <boost/filesystem.hpp>
#include "../core/wikiDataCache.h"


void read_into_vector(std::istream& istr, std::vector<std::string>& vec)
{
	std::string line;
	while(std::getline(istr, line))
		vec.push_back(line);
}


int main(int argc, char** argv)
{
	auto output_folder = boost::filesystem::path(argv[1]);

	// sort articles
	{
		std::ifstream article_titles_file((output_folder / WikiMainPath::WikiDataCache::ARTICLES_TITLES_FILE).string());
		std::ifstream article_dates_file((output_folder / WikiMainPath::WikiDataCache::ARTICLE_DATES_FILE).string());
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

		std::ofstream article_titles_output_file((output_folder / "article_titles_tmp.txt").string());
		std::ofstream article_dates_output_file((output_folder / "article_dates_tmp.txt").string());
		for (auto i : positions) {
			article_titles_output_file << article_titles[i] << std::endl;
			article_dates_output_file << article_dates[i] << std::endl;
		}
	}

	// sort categories
	{
		std::ifstream category_titles_file((output_folder / WikiMainPath::WikiDataCache::CATEGORIES_TITLES_FILE).string());
		std::vector<std::string> category_titles;
		read_into_vector(category_titles_file, category_titles);

		std::sort(category_titles.begin(), category_titles.end());

		std::ofstream category_titles_output_file((output_folder / "category_titles_tmp.txt").string());
		for (auto title : category_titles) 
			category_titles_output_file << title << std::endl;
	}

	boost::filesystem::remove(output_folder / WikiMainPath::WikiDataCache::ARTICLES_TITLES_FILE);
	boost::filesystem::remove(output_folder / WikiMainPath::WikiDataCache::ARTICLE_DATES_FILE);
	boost::filesystem::remove(output_folder / WikiMainPath::WikiDataCache::CATEGORIES_TITLES_FILE);

	boost::filesystem::rename(output_folder / "article_titles_tmp.txt", output_folder / WikiMainPath::WikiDataCache::ARTICLES_TITLES_FILE);
	boost::filesystem::rename(output_folder / "article_dates_tmp.txt", output_folder / WikiMainPath::WikiDataCache::ARTICLE_DATES_FILE);
	boost::filesystem::rename(output_folder / "category_titles_tmp.txt", output_folder / WikiMainPath::WikiDataCache::CATEGORIES_TITLES_FILE);

	return 0;
}

