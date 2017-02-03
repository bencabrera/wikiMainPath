#include "fullTextSearch.h"

#include "porter2_stemmer/porter2_stemmer.h"

typedef std::map<std::string, std::set<std::size_t>> InvertedIndex;
typedef std::set<std::size_t> DocumentSet;

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
	std::vector<std::string> queryWordVec;
	boost::split(queryWordVec, queryStr, boost::is_any_of(" "));

	for (auto& i : queryWordVec) {
		i = stem(i);	
	}

	std::size_t i = 0;
	while(i < queryWordVec.size() && invertedIndex.find(queryWordVec[i]) == invertedIndex.end())
		i++;

	auto firstIt = invertedIndex.find(queryWordVec[i]);
	if(firstIt == invertedIndex.end())
		return DocumentSet();

	auto curDocuments = firstIt->second;
	for(std::size_t a = i; a < queryWordVec.size(); a++)
	{
		auto it = invertedIndex.find(queryWordVec[a]);
		if(it != invertedIndex.end())
		{
			DocumentSet tmp;
			std::set_intersection(curDocuments.begin(), curDocuments.end(), it->second.begin(), it->second.end(), std::inserter(tmp, tmp.begin()));
			curDocuments = tmp;
		}
	}

	return curDocuments;
}
