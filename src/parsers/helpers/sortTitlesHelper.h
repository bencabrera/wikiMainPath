#pragma once

#include <iostream>
#include <vector>

void sort_article_files(std::istream& article_titles_file, std::istream& article_dates_file, std::ostream& article_titles_output_file, std::ostream& article_dates_output_file);
void sort_category_files(std::istream& category_titles_file, std::ostream& category_titles_output_file);
