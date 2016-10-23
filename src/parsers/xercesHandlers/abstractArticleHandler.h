#pragma once

#include "articleData.h"

class AbstractArticleHandler {
	public:
		virtual void HandleArticle(const ArticleData&) = 0;
};
