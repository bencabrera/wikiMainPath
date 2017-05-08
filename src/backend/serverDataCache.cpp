#include "serverDataCache.h"

#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include "graphDrawing/randomGraphDrawing.h"
#include "graphDrawing/forceDirectedGraphDrawing.h"
#include "graphDrawing/averagedPrecessorGraphDrawing.h"
#include <iostream>
#include "../core/dateDifference.h"

#include "../core/buildOneCategoryRecursively.h"

#include "../../libs/main_path_analysis/src/mainPathAnalysis/checkGraphProperties.hpp"
#include "../../libs/main_path_analysis/src/mainPathAnalysis/edgeWeightGeneration.hpp"
#include "../../libs/main_path_analysis/src/mainPathAnalysis/mpaAlgorithms.hpp"
#include "../../libs/main_path_analysis/src/mainPathAnalysis/preAndPostProcessing.hpp"

ServerDataCache::ServerDataCache(const WikiMainPath::WikiDataCache& wiki_data_cache)
	:_wiki_data_cache(wiki_data_cache),
	_article_titles(wiki_data_cache.article_titles()),
	_category_titles(wiki_data_cache.category_titles()),
	_article_dates(wiki_data_cache.article_dates()),
	_category_has_article(wiki_data_cache.category_has_article()),
	_article_network(wiki_data_cache.article_network()),
	_category_hirachy_graph(wiki_data_cache.category_hirachy_graph())
{}


const std::vector<std::size_t>& ServerDataCache::get_article_list(std::size_t category_id)
{
	if(_article_list_cache.count(category_id) == 0)
	{
		Poco::Mutex::ScopedLock lock(_mutices[category_id % N_MUTEX]);	
		compute_article_list(category_id);	
		_article_list_priority_list.push_front(category_id);

		auto which_to_delete = _article_list_priority_list.back();
		if(_article_list_priority_list.size() > MAX_CACHE_SIZE)
		{
			Poco::Mutex::ScopedLock lock(_mutices[which_to_delete % N_MUTEX]);	
			_article_list_cache.erase(which_to_delete);
			_article_list_priority_list.pop_back();
		}
	}

	return _article_list_cache.at(category_id);
}

const ArticleGraph& ServerDataCache::get_event_network(std::size_t category_id)
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

const ServerDataCache::EdgeList& ServerDataCache::get_global_main_path(std::size_t category_id)
{
	if(_global_main_path_cache.count(category_id) == 0)
	{
		Poco::Mutex::ScopedLock lock(_mutices[category_id % N_MUTEX]);	
		compute_global_main_path(category_id);	
		_global_main_path_priority_list.push_front(category_id);

		auto which_to_delete = _global_main_path_priority_list.back();
		if(_global_main_path_priority_list.size() > MAX_CACHE_SIZE)
		{
			Poco::Mutex::ScopedLock lock(_mutices[which_to_delete % N_MUTEX]);	
			_global_main_path_cache.erase(which_to_delete);
			_global_main_path_priority_list.pop_back();
		}
	}

	return _global_main_path_cache.at(category_id);
}


// ---- private ----

void ServerDataCache::compute_article_list(std::size_t category_id)
{
	// build recursively all articles in category
	auto articles_in_category = build_one_category_recursively(category_id, _category_hirachy_graph, _category_has_article);
	std::vector<std::size_t> rtn(articles_in_category.begin(), articles_in_category.end()); 

	rtn.erase(
		std::remove_if(rtn.begin(), rtn.end(), [this](std::size_t i_article)
		{
			for (auto& f : article_filters) {
				if(!f(_article_titles[i_article], _article_dates[i_article]))
					return true;		
			}
			return false;
		}), 
		rtn.end()
	);
	_article_list_cache.insert({ category_id, rtn });
}

void ServerDataCache::compute_event_network(std::size_t category_id)
{
	const auto& article_list = get_article_list(category_id);

	std::map<std::size_t,std::pair<std::size_t,std::size_t>> events_in_category;	
	std::size_t cur_event_id = 0;
	for (auto article_id : article_list) {
		events_in_category.insert({ article_id, { cur_event_id, cur_event_id + _article_dates[article_id].size() - 1 } });
		cur_event_id += _article_dates[article_id].size();
	}

	std::set<std::size_t> articles_in_network;
	for(auto& ev : events_in_category) 
		articles_in_network.insert(ev.first);		

	ArticleGraph g(cur_event_id);
	
	for(auto& ev : events_in_category)
	{
		std::vector<std::size_t> neighbors_in_network;
		std::set_intersection(_article_network[ev.first].begin(), _article_network[ev.first].end(), articles_in_network.begin(), articles_in_network.end(), std::back_inserter(neighbors_in_network));		

		for(std::size_t event_1 = ev.second.first; event_1 <= ev.second.second; event_1++)
			for(auto neighbor_article : neighbors_in_network)
				for(std::size_t event_2 = events_in_category.at(neighbor_article).first; event_2 <= events_in_category.at(neighbor_article).second; event_2++)
				{
					auto& date_1 = _article_dates[ev.first][event_1 - ev.second.first];
					auto& date_2 = _article_dates[neighbor_article][event_2 - events_in_category.at(neighbor_article).first];
					if((date_1 == date_2 && event_1 < event_2) || date_1 < date_2)
					{
						if(!boost::edge(event_1, event_2, g).second)
							boost::add_edge(event_1, event_2, g);
					}
					else
					{
						if(!boost::edge(event_2, event_1, g).second)
							boost::add_edge(event_2, event_1, g);
					}
				}
	}

	_event_network_cache.insert({ category_id, g });
}

void ServerDataCache::compute_event_list(std::size_t category_id)
{
	const auto& article_list = get_article_list(category_id);

	EventList event_list;
	for (auto article_id : article_list) {
		for (auto d : _article_dates[article_id]) {
			std::string article_title = boost::to_upper_copy(d.Description) + ": " + _article_titles[article_id];
			event_list.push_back(Event{ article_title, d.Begin });
		}
	}

	_event_list_cache.insert({ category_id, std::move(event_list) });
}

std::vector<double> ServerDataCache::compute_x_positions(const EventList& event_list)
{
	auto min_event_it = std::min_element(event_list.begin(), event_list.end(), [](const Event& e1, const Event& e2) {
		return e1.Date < e2.Date;
	});

	std::size_t max_difference = 0;
	std::vector<std::size_t> day_differences;
	for (auto event : event_list) 
	{
		auto diff = difference_in_days(event.Date, min_event_it->Date);
		max_difference = std::max(max_difference, diff);
		day_differences.push_back(diff);
	}

	std::vector<double> x_positions;
	for (auto diff : day_differences) 
		x_positions.push_back(diff / static_cast<double>(max_difference));

	return x_positions;
}

void ServerDataCache::compute_network_positions(std::size_t category_id)
{
	const auto& event_network = get_event_network(category_id);
	// const auto& event_list = get_event_list(category_id);

	// get normalized x coordinates for drawing algorithms
	// auto x_positions = compute_x_positions(event_list);

	// auto positions = WikiMainPath::GraphDrawing::force_directed_graph_drawing(event_network, x_positions);
	auto positions = WikiMainPath::GraphDrawing::averaged_precessor_graph_drawing(event_network);
	// auto positions = averaged_precessor_graph_drawing(event_network);
	_network_positions_cache.insert({ category_id, std::move(positions) });
}

void ServerDataCache::compute_global_main_path(std::size_t category_id)
{
	const auto& network = get_event_network(category_id);

	ArticleGraph g;
	boost::copy_graph(network, g);
	MainPathAnalysis::set_increasing_edge_index(g);

	// add s and t vertex
	ArticleGraph::vertex_descriptor s, t;
	MainPathAnalysis::add_source_and_sink_vertex<ArticleGraph>(g, s, t);

	// compute spc weights
	auto weights = MainPathAnalysis::generate_spc_weights(g, s, t);

	// compute global main path
	std::vector<ArticleGraph::edge_descriptor> main_path;
	MainPathAnalysis::global(std::back_inserter(main_path), g, weights, s, t);

	// remove s and t from main path and from copy of graph
	MainPathAnalysis::remove_edges_containing_source_or_sink(g, s, t, main_path);
	MainPathAnalysis::remove_source_and_sink_vertex(g, s, t);

	// build json object of main path
	EdgeList main_path_edges;
	for (auto e : main_path) 
		main_path_edges.push_back({ boost::source(e,g), boost::target(e,g) });

	_global_main_path_cache.insert({ category_id, std::move(main_path_edges) });
}
