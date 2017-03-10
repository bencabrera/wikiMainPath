#include "articleDatesAndCategoriesHandler.h"

#include <boost/algorithm/string/trim.hpp>

#include "../date/articleDateExtraction.h"
#include "../date/infoboxDateExtraction.h"

ArticleDatesAndCategoriesHandler::ArticleDatesAndCategoriesHandler(std::ostream* report_ostr)
:ExtractOnlyArticlesWithDates(true),
n_errors(0),
_report_ostr(report_ostr)
{}

void ArticleDatesAndCategoriesHandler::HandleArticle(const WikiXmlDumpXerces::WikiPageData& data)
{
	std::string title = data.MetaData.at("title");
	boost::trim(title);

	// if it is redirect, safe it to the redirects map for later
	if(data.IsRedirect)
	{
		redirects.insert({ title, data.RedirectTarget });
	}
	else
	{
		if(title.size() > 9 && title.substr(0,9) == "Category:")
		{
			categories.push_back(title.substr(9));
		}
		else
		{
			std::vector<WikiMainPath::InfoboxDateExtractionError> errors;
			auto extracted_dates = WikiMainPath::extract_all_dates_from_article(data.Content, errors);
			if(extracted_dates.size() > 0 || !ExtractOnlyArticlesWithDates)
			{
				articles.insert({ title, extracted_dates });
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
