#include "articleDatesAndCategoriesHandler.h"

#include <boost/algorithm/string/trim.hpp>

#include "../date/articleDateExtraction.h"
#include "../date/infoboxDateExtraction.h"

ArticleDatesAndCategoriesHandler::ArticleDatesAndCategoriesHandler(
	std::ostream& article_titles_file,
	std::ostream& article_dates_file,
	std::ostream& category_titles_file,
	std::ostream& redirect_file,
	std::mutex& article_titles_mutex,
	std::mutex& category_titles_mutex,
	std::mutex& redirect_mutex,
	std::ostream* report_ostr
)
	:ExtractOnlyArticlesWithDates(true),
	n_errors(0),
	_report_ostr(report_ostr),
	_article_titles_file(article_titles_file),
	_article_dates_file(article_dates_file),
	_category_titles_file(category_titles_file),
	_redirect_file(redirect_file),
	_article_titles_mutex(article_titles_mutex),
	_category_titles_mutex(category_titles_mutex),
	_redirect_mutex(redirect_mutex)
{}

ArticleDatesAndCategoriesHandler::~ArticleDatesAndCategoriesHandler()
{
	write_article_titles();
	write_category_titles();
	write_redirects();
}

void ArticleDatesAndCategoriesHandler::write_article_titles()
{
	std::lock_guard<std::mutex> lock(_article_titles_mutex);
	for(auto article : _articles)
	{
		_article_titles_file << article.first << std::endl;

		for (auto date : article.second) {
			_article_dates_file << Date::serialize(date) << '\t';
		}
		_article_dates_file << std::endl;
	}
	// _articles.clear();
}

void ArticleDatesAndCategoriesHandler::write_category_titles()
{
	std::lock_guard<std::mutex> lock(_category_titles_mutex);
	for(auto title : _categories)
		_category_titles_file << title << std::endl;
	_categories.clear();
}

void ArticleDatesAndCategoriesHandler::write_redirects()
{
	std::lock_guard<std::mutex> lock(_redirect_mutex);
	for (auto redirect : _redirects) 
		_redirect_file << redirect.first << "\t" << redirect.second << std::endl;	
	_redirects.clear();
}

void ArticleDatesAndCategoriesHandler::HandleArticle(const WikiXmlDumpXerces::WikiPageData& data)
{
	std::string title = data.MetaData.at("title");
	boost::trim(title);

	// if it is redirect, safe it to the redirects map for later
	if(data.IsRedirect)
	{
		_redirects.insert({ title, data.RedirectTarget });
		if(_redirects.size() % WRITE_INTERVAL == 0)
			write_redirects();
	}
	else
	{
		if(title.size() > 9 && title.substr(0,9) == "Category:")
		{
			_categories.push_back(title.substr(9));
			if(_categories.size() % WRITE_INTERVAL == 0)
				write_category_titles();
		}
		else
		{
			std::vector<WikiMainPath::InfoboxDateExtractionError> errors;
			auto extracted_dates = WikiMainPath::extract_all_dates_from_article(data.Content, errors);
			if(extracted_dates.size() > 0 || !ExtractOnlyArticlesWithDates)
			{
				_articles.insert({ title, extracted_dates });
				if(_articles.size() % WRITE_INTERVAL == 0)
					write_article_titles();
			}

			n_errors += errors.size();

			// report errors / mismatches
			if(_report_ostr != nullptr) {
				for (auto err : errors) 
					if(std::get<0>(err) == WikiMainPath::KEY_BUT_NO_DATE_EXTRACTED)
						*_report_ostr << WikiMainPath::InfoboxDateExtractionErrorLabel[std::get<0>(err)] << " --- '" << std::get<1>(err) << "' --- '" << std::get<2>(err) << "'" << std::endl;
			}
		}
	}
}
