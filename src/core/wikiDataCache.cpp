#include "wikiDataCache.h"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace WikiMainPath {

	constexpr char WikiDataCache::ARTICLES_TITLES_FILE[];
	constexpr char WikiDataCache::CATEGORIES_TITLES_FILE[];
	constexpr char WikiDataCache::REDIRECTS_FILE[];
	constexpr char WikiDataCache::ARTICLE_DATES_FILE[];
	constexpr char WikiDataCache::CAT_HAS_ARTICLE_FILE[];
	constexpr char WikiDataCache::ARTICLE_NETWORK_FILE[];
	constexpr char WikiDataCache::EVENT_INDICES_FILE[];
	constexpr char WikiDataCache::EVENT_NETWORK_FILE[];

	WikiDataCache::WikiDataCache(std::string folder)
		:_folder(folder)
	{
		if(!boost::filesystem::is_directory(_folder))
			boost::filesystem::create_directory(_folder);
	}



	// getter methods
	const std::vector<std::string>& WikiDataCache::article_titles() 
	{
		if(_article_titles == nullptr)
			_article_titles = read_article_titles((_folder / ARTICLES_TITLES_FILE).string());	

		return *_article_titles;
	}

	const std::vector<std::string>& WikiDataCache::category_titles() 
	{
		if(_category_titles == nullptr)
			_category_titles = read_category_titles((_folder / CATEGORIES_TITLES_FILE).string());	

		return *_category_titles;
	}

	const std::map<std::string,std::string>& WikiDataCache::redirects()
	{
		if(_redirects == nullptr)
			_redirects = read_redirects((_folder / REDIRECTS_FILE).string());	

		return *_redirects;
	}

	const std::vector<std::vector<Date>>& WikiDataCache::article_dates()
	{
		if(_article_dates == nullptr)
			_article_dates = read_article_dates((_folder / ARTICLE_DATES_FILE).string());	

		return *_article_dates;
	}

	const std::vector<std::vector<std::size_t>>& WikiDataCache::category_has_article()
	{
		if(_category_has_article == nullptr)
			_category_has_article = read_category_has_article((_folder / CAT_HAS_ARTICLE_FILE).string());	

		return *_category_has_article;
	}

	const std::vector<boost::container::flat_set<std::size_t>>& WikiDataCache::category_has_article_set()
	{
		if(_category_has_article_set == nullptr)
			_category_has_article_set = read_category_has_article_set((_folder / CAT_HAS_ARTICLE_FILE).string());	

		return *_category_has_article_set;
	}

	const std::vector<std::vector<std::size_t>>& WikiDataCache::article_network()
	{
		if(_article_network == nullptr)
			_article_network = read_article_network((_folder / ARTICLE_NETWORK_FILE).string());	

		return *_article_network;
	}

	// const std::vector<std::size_t>& WikiDataCache::event_indices()
	// {
		// if(_event_indices == nullptr)
			// _event_indices = read_event_indices((_folder / EVENT_INDICES_FILE).string());	

		// return *_event_indices;
	// }

	// const EventNetwork& WikiDataCache::event_network()
	// {
		// if(_event_network == nullptr)
			// _event_network = read_event_network((_folder / EVENT_NETWORK_FILE).string());	

		// return *_event_network;
	// }



	// const getter methods
	const std::vector<std::string>& WikiDataCache::article_titles() const
	{
		if(_article_titles == nullptr)
			throw std::logic_error("Accessed article_titles without reading it first");

		return *_article_titles;
	}

	const std::vector<std::string>& WikiDataCache::category_titles() const
	{
		if(_category_titles == nullptr)
			throw std::logic_error("Accessed category_titles without reading it first");

		return *_category_titles;
	}

	const std::map<std::string,std::string>& WikiDataCache::redirects() const
	{
		if(_redirects == nullptr)
			throw std::logic_error("Accessed redirects without reading it first");

		return *_redirects;
	}

	const std::vector<std::vector<Date>>& WikiDataCache::article_dates() const
	{
		if(_article_dates == nullptr)
			throw std::logic_error("Accessed article_dates without reading it first");

		return *_article_dates;
	}

	const std::vector<std::vector<std::size_t>>& WikiDataCache::category_has_article() const
	{
		if(_category_has_article == nullptr)
			throw std::logic_error("Accessed category_has_article without reading it first");

		return *_category_has_article;
	}

	const std::vector<boost::container::flat_set<std::size_t>>& WikiDataCache::category_has_article_set() const
	{
		if(_category_has_article_set == nullptr)
			throw std::logic_error("Accessed category_has_article_set without reading it first");

		return *_category_has_article_set;
	}

	const std::vector<std::vector<std::size_t>>& WikiDataCache::article_network() const
	{
		if(_article_network == nullptr)
			throw std::logic_error("Accessed article_network without reading it first");

		return *_article_network;
	}

	// const std::vector<std::size_t>& WikiDataCache::event_indices() const
	// {
		// if(_event_indices == nullptr)
			// throw std::logic_error("Accessed event_indices without reading it first");

		// return *_event_indices;
	// }

	// const EventNetwork& WikiDataCache::event_network() const
	// {
		// if(_event_network == nullptr)
			// throw std::logic_error("Accessed event_network without reading it first");

		// return *_event_network;
	// }




	// file reader methods
	std::unique_ptr<std::vector<std::string>> WikiDataCache::read_article_titles(std::string articles_file_path)
	{
		std::ifstream file(articles_file_path);	
		if(!file.is_open())
			throw std::logic_error("Article file not found");

		std::unique_ptr<std::vector<std::string>> rtn(new std::vector<std::string>());

		std::string line;
		while(std::getline(file, line))
			rtn->push_back(line);

		for(std::size_t i = 0; i < std::min(rtn->size(),10ul); i++)
			if((*rtn)[i] >= (*rtn)[i+1])
				throw std::logic_error("article_titles are not ordered");

		return rtn;
	}

	std::unique_ptr<std::vector<std::string>> WikiDataCache::read_category_titles(std::string categories_file_path)
	{
		std::ifstream file(categories_file_path);	
		if(!file.is_open())
			throw std::logic_error("Categories file not found");

		std::unique_ptr<std::vector<std::string>> rtn(new std::vector<std::string>());

		std::string line;
		while(std::getline(file, line))
			rtn->push_back(line);

		for(std::size_t i = 0; i < std::min(rtn->size(),10ul); i++)
			if((*rtn)[i] >= (*rtn)[i+1])
				throw std::logic_error("category_titles are not ordered");

		return rtn;
	}

	std::unique_ptr<std::map<std::string,std::string>> WikiDataCache::read_redirects(std::string redirects_file_path)
	{
		std::ifstream file(redirects_file_path);	
		if(!file.is_open())
			throw std::logic_error("Redirects file not found");

		std::unique_ptr<std::map<std::string,std::string>> rtn(new std::map<std::string,std::string>());

		std::string line;
		while(std::getline(file, line))
		{
			std::vector<std::string> strs;
			boost::split(strs,line,boost::is_any_of("\t"));
			rtn->insert({ strs.at(0), strs.at(1) });
		}

		return rtn;
	}

	std::unique_ptr<std::vector<std::vector<Date>>> WikiDataCache::read_article_dates(std::string dates_file_path)
	{
		std::ifstream dates_file(dates_file_path);	
		if(!dates_file.is_open())
			throw std::logic_error("Article dates file not found");

		std::unique_ptr<std::vector<std::vector<Date>>> rtn(new std::vector<std::vector<Date>>());

		std::string line;
		while(std::getline(dates_file, line))
		{
			std::vector<std::string> date_strs;
			std::vector<Date> dates;
			boost::split(date_strs,line,boost::is_any_of("\t"));

			for (auto& date_str : date_strs) 
				if(date_str != "")
					dates.push_back(Date::deserialize(date_str));

			rtn->push_back(dates);
		}

		return rtn;
	}

	std::unique_ptr<std::vector<std::vector<std::size_t>>> WikiDataCache::read_category_has_article(std::string category_has_article_file_path)
	{
		std::ifstream category_has_article_file(category_has_article_file_path);	
		if(!category_has_article_file.is_open())
			throw std::logic_error("Category_has_article file not found");

		std::unique_ptr<std::vector<std::vector<std::size_t>>> category_has_article(new std::vector<std::vector<std::size_t>>());

		std::string line;
		while(std::getline(category_has_article_file, line))
		{
			std::vector<std::size_t> linked_articles;	

			std::istringstream ss(line);
			while(!ss.eof())
			{
				std::string tmpStr;
				ss >> tmpStr;
				if(tmpStr != "")
					linked_articles.push_back(std::stoi(tmpStr));
			}

			category_has_article->push_back(std::move(linked_articles));
		}

		return category_has_article;
	}

	std::unique_ptr<std::vector<boost::container::flat_set<std::size_t>>> WikiDataCache::read_category_has_article_set(std::string category_has_article_file_path)
	{
		std::ifstream category_has_article_file(category_has_article_file_path);	
		if(!category_has_article_file.is_open())
			throw std::logic_error("Category_has_article file not found");

		std::unique_ptr<std::vector<boost::container::flat_set<std::size_t>>> category_has_article(new std::vector<boost::container::flat_set<std::size_t>>());

		std::string line;
		while(std::getline(category_has_article_file, line))
		{
			boost::container::flat_set<std::size_t> linked_articles;	

			std::istringstream ss(line);
			while(!ss.eof())
			{
				std::string tmpStr;
				ss >> tmpStr;
				if(tmpStr != "")
					linked_articles.insert(std::stoi(tmpStr));
			}

			category_has_article->push_back(std::move(linked_articles));
		}

		return category_has_article;
	}


	std::unique_ptr<std::vector<std::vector<std::size_t>>> WikiDataCache::read_article_network(std::string article_network_path)
	{
		std::ifstream adj_list_file(article_network_path);	
		if(!adj_list_file.is_open())
			throw std::logic_error("Article network file not found");

		std::unique_ptr<std::vector<std::vector<std::size_t>>> adj_list(new std::vector<std::vector<std::size_t>>());

		std::string line;
		while(std::getline(adj_list_file, line))
		{
			std::vector<std::size_t> neighbors;
			std::istringstream ss(line);
			while(!ss.eof())
			{
				std::string tmpStr;
				ss >> tmpStr;
				if(tmpStr != "")
				{
					std::size_t target = std::stoi(tmpStr);
					neighbors.push_back(target);
				}
			}
			adj_list->push_back(std::move(neighbors));
		}

		return adj_list;
	}

	// std::unique_ptr<std::vector<std::size_t>> WikiDataCache::read_event_indices(std::string event_indices_file_path)
	// {
		// std::ifstream file(event_indices_file_path);	
		// if(!file.is_open())
			// throw std::logic_error("Event indices file not found");

		// std::unique_ptr<std::vector<std::size_t>> rtn(new std::vector<std::size_t>());

		// std::size_t tmp;
		// while(!file.eof())
		// {
			// file >> tmp;
			// rtn->push_back(tmp);
		// }

		// return rtn;
	// }

	// std::unique_ptr<ArticleNetwork> WikiDataCache::read_event_network(std::string event_network_path)
	// {
		// std::ifstream adj_list_file(event_network_path);	
		// if(!adj_list_file.is_open())
			// throw std::logic_error("Article network file not found");

		// std::unique_ptr<ArticleNetwork> graph(new EventNetwork());

		// std::size_t source = 0;
		// std::string line;
		// while(std::getline(adj_list_file, line))
		// {
			// std::istringstream ss(line);
			// while(!ss.eof())
			// {
				// std::string tmpStr;
				// ss >> tmpStr;
				// if(tmpStr != "")
				// {
					// std::size_t target = std::stoi(tmpStr);
					// boost::add_edge(source, target, *graph);
				// }
			// }

			// source++;
		// }

		// return graph;
	// }



	// file writer methods
	void WikiDataCache::write_article_titles(const std::map<std::string,std::vector<Date>>& articles_with_dates)
	{
		std::ofstream articles_file((_folder / ARTICLES_TITLES_FILE).string());	
		for(auto article : articles_with_dates)
			articles_file << article.first << std::endl;
	}

	void WikiDataCache::write_category_titles(const std::vector<std::string>& categories)
	{
		std::ofstream file((_folder / CATEGORIES_TITLES_FILE).string());	
		for(auto title : categories)
			file << title << std::endl;
	}

	void WikiDataCache::write_redirects(std::map<std::string,std::string> redirects)
	{
		std::ofstream redirects_file((_folder / REDIRECTS_FILE).string());	
		for (auto redirect : redirects) 
			redirects_file << redirect.first << "\t" << redirect.second << std::endl;	
	}

	void WikiDataCache::write_article_dates(const std::map<std::string,std::vector<Date>>& articles_with_dates)
	{
		std::ofstream article_dates_file((_folder / ARTICLE_DATES_FILE).string());	
		for(auto article : articles_with_dates)
		{
			for (auto date : article.second) {
				article_dates_file << Date::serialize(date) << '\t';
			}
			article_dates_file << std::endl;
		}
	}

	void WikiDataCache::write_category_has_article(const std::vector<boost::container::flat_set<std::size_t>>& cat_has_art)
	{
		std::ofstream catArtFile((_folder / CAT_HAS_ARTICLE_FILE).string());	
		for(std::size_t i = 0; i < cat_has_art.size(); i++)
		{
			for (auto art : cat_has_art[i]) 
				catArtFile << art << " ";	
			catArtFile << std::endl;
		}
	}

	void WikiDataCache::write_article_network(const std::vector<boost::container::flat_set<std::size_t>>& adj_list)
	{
		std::ofstream graphFile((_folder / ARTICLE_NETWORK_FILE).string());	
		for (auto arts : adj_list) 
		{
			for (auto art : arts) 
				graphFile << art << " ";

			graphFile << std::endl;
		}
	}

	// void WikiDataCache::write_event_indices(const std::vector<std::size_t>& event_indices)
	// {
		// std::ofstream file((_folder / EVENT_INDICES_FILE).string());	
		// for(auto idx : event_indices)
			// file << idx << std::endl;
	// }

	// void WikiDataCache::write_event_network(const std::vector<boost::container::flat_set<std::size_t>>& adj_list)
	// {
		// std::ofstream graphFile((_folder / EVENT_NETWORK_FILE).string());	
		// for (auto arts : adj_list) 
		// {
			// for (auto art : arts) 
				// graphFile << art << " ";

			// graphFile << std::endl;
		// }
	// }

}
