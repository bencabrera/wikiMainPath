#include "serverDataCache.h"

#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include "graphDrawing/randomGraphDrawing.h"
#include <iostream>

ServerDataCache::ServerDataCache(const WikiMainPath::WikiDataCache& wiki_data_cache)
	:_wiki_data_cache(wiki_data_cache),
	_article_titles(wiki_data_cache.article_titles()),
	_category_titles(wiki_data_cache.category_titles()),
	_article_dates(wiki_data_cache.article_dates()),
	_category_has_article(wiki_data_cache.category_has_article()),
	_event_indices(wiki_data_cache.event_indices()),
	_event_network(wiki_data_cache.event_network())
{}



const EventNetwork& ServerDataCache::get_event_network(std::size_t category_id)
{
	if(_event_network_cache.count(category_id) == 0)
	{
		Poco::Mutex::ScopedLock lock(_mutices[category_id % N_MUTEX]);	
		compute_event_network(category_id);	
		_event_network_priority_list.push_front(category_id);

		auto which_to_delete = _event_network_priority_list.back();
		if(_event_network_priority_list.size() > MAX_CACHE_SIZE)
		{
			Poco::Mutex::ScopedLock lock(_mutices[which_to_delete % N_MUTEX]);	
			_event_network_cache.erase(which_to_delete);
			_event_network_priority_list.pop_back();
		}
	}

	return _event_network_cache.at(category_id);
}


const std::vector<ServerDataCache::Event>& ServerDataCache::get_event_list(std::size_t category_id)
{
	if(_event_list_cache.count(category_id) == 0)
	{
		Poco::Mutex::ScopedLock lock(_mutices[category_id % N_MUTEX]);	
		compute_event_list(category_id);	
		_event_list_priority_list.push_front(category_id);

		auto which_to_delete = _event_list_priority_list.back();
		if(_event_list_priority_list.size() > MAX_CACHE_SIZE)
		{
			Poco::Mutex::ScopedLock lock(_mutices[which_to_delete % N_MUTEX]);	
			_event_list_cache.erase(which_to_delete);
			_event_list_priority_list.pop_back();
		}
	}

	return _event_list_cache.at(category_id);
}


const std::vector<double>& ServerDataCache::get_network_positions(std::size_t category_id)
{
	if(_network_positions_cache.count(category_id) == 0)
	{
		Poco::Mutex::ScopedLock lock(_mutices[category_id % N_MUTEX]);	
		compute_network_positions(category_id);	
		_network_positions_priority_list.push_front(category_id);

		auto which_to_delete = _network_positions_priority_list.back();
		if(_network_positions_priority_list.size() > MAX_CACHE_SIZE)
		{
			Poco::Mutex::ScopedLock lock(_mutices[which_to_delete % N_MUTEX]);	
			_network_positions_cache.erase(which_to_delete);
			_network_positions_priority_list.pop_back();
		}
	}

	return _network_positions_cache.at(category_id);
}


// ---- private ----

void ServerDataCache::compute_event_network(std::size_t category_id)
{
	std::vector<std::size_t> events_in_category;	
	for (auto article_id : _category_has_article[category_id]) {
		for(std::size_t idx = _event_indices[article_id]; idx < _event_indices[article_id+1]; idx++)
			events_in_category.push_back(idx);
	}

	EventNetwork subgraph = _event_network.create_subgraph();
	for (auto i : events_in_category) 
		boost::add_vertex(i, subgraph);	

	_event_network_cache.insert({ category_id, subgraph });

	boost::copy_graph(subgraph,_event_network_cache.at(category_id)); // needed because otherwise it looses all its vetices, WTF
}

void ServerDataCache::compute_event_list(std::size_t category_id)
{
	const auto& event_network = get_event_network(category_id);

	EventList event_list;
	for (auto v : boost::make_iterator_range(boost::vertices(event_network))) {

		auto global_event_id = event_network.local_to_global(v);

		auto article_it = std::lower_bound(_event_indices.begin(), _event_indices.end(), global_event_id);	
		std::size_t article_id = article_it - _event_indices.begin();
		if(_event_indices[article_id] != global_event_id)
			article_id -= 1;
		std::size_t date_id = global_event_id - _event_indices[article_id];

		std::string article_title = boost::to_upper_copy(_article_dates[article_id][date_id].Description) + ": " + _article_titles[article_id];

		event_list.push_back(Event{ article_title, global_event_id, to_iso_string(_article_dates[article_id][date_id].Begin) });
	}

	_event_list_cache.insert({ category_id, std::move(event_list) });
}

void ServerDataCache::compute_network_positions(std::size_t category_id)
{
	const auto& event_network = get_event_network(category_id);
	auto positions = random_graph_drawing(event_network);
	_network_positions_cache.insert({ category_id, std::move(positions) });
}
