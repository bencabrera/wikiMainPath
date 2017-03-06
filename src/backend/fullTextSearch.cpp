#include "fullTextSearch.h"

#include "../../libs/porter2_stemmer/porter2_stemmer.h"

namespace WikiMainPath {

	std::string stem(std::string str)
	{
		Porter2Stemmer::stem(str);
		return boost::to_lower_copy(str);
	}

	InvertedIndex buildInvertedIndex(const std::vector<std::string>& documents)
	{
		InvertedIndex rtn;

		std::size_t iDoc = 0;
		for (auto doc : documents) {
			std::vector<std::string> wordVec;
			boost::split(wordVec, doc, boost::is_any_of(" "));
			for (auto wordStr : wordVec) {
				// stop words and stemming
				std::string word = stem(wordStr);


				// add to inverted index
				auto it = rtn.find(word);
				if(it != rtn.end())
					it->second.insert(iDoc);
				else
					rtn.insert({ word, { iDoc } });
			}

			iDoc++;
		}

		return rtn;
	}

	DocumentSet query(const InvertedIndex& invertedIndex, std::string queryStr)
	{
		std::vector<std::string> query_word_vec;
		boost::split(query_word_vec, queryStr, boost::is_any_of(" "));

		std::vector<std::string> stemmed_query_word_vec;
		for (auto& i : query_word_vec) 
			stemmed_query_word_vec.push_back(stem(i));	

		// iterate over vec until a word is recognized
		std::size_t i = 0;
		while(i < stemmed_query_word_vec.size() && invertedIndex.find(stemmed_query_word_vec[i]) == invertedIndex.end())
			i++;

		if(i >= stemmed_query_word_vec.size())
			return DocumentSet();

		auto firstIt = invertedIndex.find(stemmed_query_word_vec[i]);
		if(firstIt == invertedIndex.end())
			return DocumentSet();

		auto curDocuments = firstIt->second;
		for(std::size_t a = i; a < stemmed_query_word_vec.size(); a++)
		{
			auto it = invertedIndex.find(stemmed_query_word_vec[a]);
			if(it != invertedIndex.end())
			{
				DocumentSet tmp;
				std::set_intersection(curDocuments.begin(), curDocuments.end(), it->second.begin(), it->second.end(), std::inserter(tmp, tmp.begin()));
				curDocuments = tmp;
			}
		}

		return curDocuments;
	}

}
