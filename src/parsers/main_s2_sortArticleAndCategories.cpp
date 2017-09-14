#include <fstream>
#include <algorithm>
#include <numeric>
#include <boost/filesystem.hpp>
#include "../core/wikiDataCache.h"
#include "helpers/sortTitlesHelper.h"

// boost
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char** argv)
{
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "Produce help message.")
		("output-folder", po::value<std::string>(), "The folder in which the results (articlesWithDates.txt, categories.txt, redirects.txt) should be stored.")
		;
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	// display help if --help was specified
	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return 0;
	}

	auto output_folder = boost::filesystem::path(vm["output-folder"].as<std::string>());

	// sort articles
	{
		std::ifstream article_titles_file((output_folder / WikiMainPath::WikiDataCache::ARTICLES_TITLES_FILE).string());
		std::ifstream article_dates_file((output_folder / WikiMainPath::WikiDataCache::ARTICLE_DATES_FILE).string());
		std::ofstream article_titles_output_file((output_folder / "article_titles_tmp.txt").string());
		std::ofstream article_dates_output_file((output_folder / "article_dates_tmp.txt").string());
		sort_article_files(article_titles_file, article_dates_file, article_titles_output_file, article_dates_output_file);
	}

	// sort categories
	{
		std::ifstream category_titles_file((output_folder / WikiMainPath::WikiDataCache::CATEGORIES_TITLES_FILE).string());
		std::ofstream category_titles_output_file((output_folder / "category_titles_tmp.txt").string());
		sort_category_files(category_titles_file, category_titles_output_file);
	}

	boost::filesystem::remove(output_folder / WikiMainPath::WikiDataCache::ARTICLES_TITLES_FILE);
	boost::filesystem::remove(output_folder / WikiMainPath::WikiDataCache::ARTICLE_DATES_FILE);
	boost::filesystem::remove(output_folder / WikiMainPath::WikiDataCache::CATEGORIES_TITLES_FILE);

	boost::filesystem::rename(output_folder / "article_titles_tmp.txt", output_folder / WikiMainPath::WikiDataCache::ARTICLES_TITLES_FILE);
	boost::filesystem::rename(output_folder / "article_dates_tmp.txt", output_folder / WikiMainPath::WikiDataCache::ARTICLE_DATES_FILE);
	boost::filesystem::rename(output_folder / "category_titles_tmp.txt", output_folder / WikiMainPath::WikiDataCache::CATEGORIES_TITLES_FILE);

	return 0;
}

