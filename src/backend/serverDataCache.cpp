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


const std::vector<std::size_t>& ServerDataCache::get_article_list(std::size_t category_id, const RequestParameters& request_parameters)
{
	auto hash = compute_hash(category_id, request_parameters);
	if(_article_list_cache.count(category_id) == 0)
	{
		Poco::Mutex::ScopedLock lock(_mutices[category_id % N_MUTEX]);	
		compute_article_list(category_id, request_parameters);	
		_article_list_priority_list.push_front(hash);

		auto which_to_delete = _article_list_priority_list.back();
		if(_article_list_priority_list.size() > MAX_CACHE_SIZE)
		{
			Poco::Mutex::ScopedLock lock(_mutices[which_to_delete % N_MUTEX]);	
			_article_list_cache.erase(which_to_delete);
			_article_list_priority_list.pop_back();
		}
	}

	return _article_list_cache.at(hash);
}

const std::vector<ServerDataCache::Event>& ServerDataCache::get_event_list(std::size_t category_id, const RequestParameters& request_parameters)
{
	auto hash = compute_hash(category_id, request_parameters);
	if(_event_list_cache.count(category_id) == 0)
	{
		Poco::Mutex::ScopedLock lock(_mutices[category_id % N_MUTEX]);	
		compute_event_list(category_id, request_parameters);	
		_event_list_priority_list.push_front(hash);

		auto which_to_delete = _event_list_priority_list.back();
		if(_event_list_priority_list.size() > MAX_CACHE_SIZE)
		{
			Poco::Mutex::ScopedLock lock(_mutices[which_to_delete % N_MUTEX]);	
			_event_list_cache.erase(which_to_delete);
			_event_list_priority_list.pop_back();
		}
	}

	return _event_list_cache.at(hash);
}



const EventNetwork& ServerDataCache::get_event_network(std::size_t category_id, const RequestParameters& request_parameters)
{
	auto hash = compute_hash(category_id, request_parameters);
	if(_event_network_cache.count(category_id) == 0)
	{
		Poco::Mutex::ScopedLock lock(_mutices[category_id % N_MUTEX]);	
		compute_event_network(category_id, request_parameters);	
		_event_network_priority_list.push_front(hash);

		auto which_to_delete = _event_network_priority_list.back();
		if(_event_network_priority_list.size() > MAX_CACHE_SIZE)
		{
			Poco::Mutex::ScopedLock lock(_mutices[which_to_delete % N_MUTEX]);	
			_event_network_cache.erase(which_to_delete);
			_event_network_priority_list.pop_back();
		}
	}

	return _event_network_cache.at(hash);
}



const std::vector<double>& ServerDataCache::get_network_positions(std::size_t category_id, const RequestParameters& request_parameters)
{
	auto hash = compute_hash(category_id, request_parameters);
	if(_network_positions_cache.count(category_id) == 0)
	{
		Poco::Mutex::ScopedLock lock(_mutices[category_id % N_MUTEX]);	
		compute_network_positions(category_id, request_parameters);	
		_network_positions_priority_list.push_front(hash);

		auto which_to_delete = _network_positions_priority_list.back();
		if(_network_positions_priority_list.size() > MAX_CACHE_SIZE)
		{
			Poco::Mutex::ScopedLock lock(_mutices[which_to_delete % N_MUTEX]);	
			_network_positions_cache.erase(which_to_delete);
			_network_positions_priority_list.pop_back();
		}
	}

	return _network_positions_cache.at(hash);
}

const ServerDataCache::EdgeList& ServerDataCache::get_global_main_path(std::size_t category_id, const RequestParameters& request_parameters)
{
	auto hash = compute_hash(category_id, request_parameters);
	if(_global_main_path_cache.count(category_id) == 0)
	{
		Poco::Mutex::ScopedLock lock(_mutices[category_id % N_MUTEX]);	
		compute_global_main_path(category_id, request_parameters);	
		_global_main_path_priority_list.push_front(hash);

		auto which_to_delete = _global_main_path_priority_list.back();
		if(_global_main_path_priority_list.size() > MAX_CACHE_SIZE)
		{
			Poco::Mutex::ScopedLock lock(_mutices[which_to_delete % N_MUTEX]);	
			_global_main_path_cache.erase(which_to_delete);
			_global_main_path_priority_list.pop_back();
		}
	}

	return _global_main_path_cache.at(hash);
}


// ---- private ----

void ServerDataCache::compute_article_list(std::size_t category_id, const RequestParameters& request_parameters)
{
	// build recursively all articles in category
	std::set<std::size_t> articles_in_category;
	if(request_parameters.not_recursive)
	{
		for (auto a : _category_has_article[category_id]) {
			articles_in_category.insert(a);	
		}
	}
	else
   		articles_in_category = build_one_category_recursively(category_id, _category_hirachy_graph, _category_has_article);

	std::vector<std::size_t> rtn(articles_in_category.begin(), articles_in_category.end()); 

	auto article_filters=request_parameters.article_filters();
	rtn.erase(
		std::remove_if(rtn.begin(), rtn.end(), [this, &article_filters](std::size_t i_article)
		{
			for (auto& f : article_filters) {
				if(!f(_article_titles[i_article], _article_dates[i_article]))
					return true;		
			}
			return false;
		}), 
		rtn.end()
	);
	_article_list_cache.insert({ compute_hash(category_id, request_parameters), rtn });
}


namespace {
	void add_correct_edge_between_events(std::size_t i_event_1, std::size_t i_event_2, const ServerDataCache::Event& event_1, const ServerDataCache::Event& event_2, EventNetwork& g)
	{
		if(i_event_2 != i_event_1)
		{
			if((event_1.Date == event_2.Date && i_event_1 < i_event_2) || event_1.Date < event_2.Date)
			{
				if(!boost::edge(i_event_1, i_event_2, g).second)
					boost::add_edge(i_event_1, i_event_2, g);
			}
			else
			{
				if(!boost::edge(i_event_2, i_event_1, g).second)
					boost::add_edge(i_event_2, i_event_1, g);
			}
		}
	}
}



void ServerDataCache::compute_event_list(std::size_t category_id, const RequestParameters& request_parameters)
{
	const auto& article_list = get_article_list(category_id, request_parameters);

	EventList event_list;

	auto event_filters = request_parameters.event_filters();

	for (auto article_id : article_list) {
		for (auto d : _article_dates[article_id]) {
			std::string event_title = boost::to_upper_copy(d.Description) + ": " + _article_titles[article_id];

			bool wrong = false;
			for (auto& f : event_filters) {
				if(!f(event_title,d))
				{
					wrong = true;
					break;	
				}
			}
			
			if(!wrong)
				event_list.push_back(Event{ event_title, d.Begin, article_id });
		}
	}

	std::sort(event_list.begin(), event_list.end(), [](const Event& e1, const Event& e2) {
		return e1.Date < e2.Date;
	});

	_event_list_cache.insert({ compute_hash(category_id, request_parameters), std::move(event_list) });
}


void ServerDataCache::compute_event_network(std::size_t category_id, const RequestParameters& request_parameters)
{
	const auto& event_list = get_event_list(category_id, request_parameters);

	std::map<std::size_t, std::vector<std::size_t>> events_for_article;
	std::set<std::size_t> articles_in_network;

	std::size_t i_event = 0;
	for (const auto& e : event_list) {
		auto it = events_for_article.find(e.ArticleId);
		if(it == events_for_article.end())
			events_for_article.insert({ e.ArticleId, { i_event } });
		else
			it->second.push_back(i_event);

		articles_in_network.insert(e.ArticleId);

		i_event++;
	}

	EventNetwork g(event_list.size());
	for (std::size_t i_event_1 = 0; i_event_1 < event_list.size(); i_event_1++)
	{
		const auto& event_1 = event_list[i_event_1];

		// connect event to all other events on same article page
		for (auto i_event_2 : events_for_article.at(event_1.ArticleId)) {
			const auto& event_2 = event_list[i_event_2];
			add_correct_edge_between_events(i_event_1, i_event_2, event_1, event_2, g);
		}

		// connect to all other connected article's events
		std::vector<std::size_t> neighbors_in_network;
		std::set_intersection(_article_network[event_1.ArticleId].begin(), _article_network[event_1.ArticleId].end(), articles_in_network.begin(), articles_in_network.end(), std::back_inserter(neighbors_in_network));		

		for (auto i_neighbour_article : neighbors_in_network) {
			auto it2 = events_for_article.find(i_neighbour_article);
			if(it2 != events_for_article.end())
			{
				for (auto i_event_2 : it2->second) {
					const auto& event_2 = event_list[i_event_2];
					add_correct_edge_between_events(i_event_1, i_event_2, event_1, event_2, g);
				}
			}
		}
	}

	_event_network_cache.insert({ compute_hash(category_id, request_parameters), g });
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

void ServerDataCache::compute_network_positions(std::size_t category_id, const RequestParameters& request_parameters)
{
	const auto& event_network = get_event_network(category_id, request_parameters);

	if(boost::num_vertices(event_network) == 0)
	{
		_network_positions_cache.insert({ compute_hash(category_id, request_parameters), std::vector<double>() });
		return;
	}
	// const auto& event_list = get_event_list(category_id, request_parameters);

	// get normalized x coordinates for drawing algorithms
	// auto x_positions = compute_x_positions(event_list);

	// auto positions = WikiMainPath::GraphDrawing::force_directed_graph_drawing(event_network, x_positions);
	// auto positions = WikiMainPath::GraphDrawing::averaged_precessor_graph_drawing(event_network);
	auto positions = WikiMainPath::GraphDrawing::random_graph_drawing(event_network);
	_network_positions_cache.insert({ compute_hash(category_id, request_parameters), std::move(positions) });
}

void ServerDataCache::compute_global_main_path(std::size_t category_id, const RequestParameters& request_parameters)
{
	const auto& network = get_event_network(category_id, request_parameters);
	if(boost::num_vertices(network) < 3 || boost::num_edges(network) < 2)
	{
		_global_main_path_cache.insert({ compute_hash(category_id, request_parameters), EdgeList() });
		return;
	}

	EventNetwork g;
	boost::copy_graph(network, g);
	MainPathAnalysis::set_increasing_edge_index(g);

	// add s and t vertex
	EventNetwork::vertex_descriptor s, t;
	MainPathAnalysis::add_source_and_sink_vertex<EventNetwork>(g, s, t);

	// compute spc weights
	// auto weights = MainPathAnalysis::generate_spc_weights(g, s, t);
	auto weights = MainPathAnalysis::generate_spc_weights_big_int(g, s, t);

	// compute global main path
	std::vector<EventNetwork::edge_descriptor> main_path;
	// double alpha = 1;
	// do {
		// main_path.clear();
		// // MainPathAnalysis::localForward(std::back_inserter(main_path), g, weights, s, t);
		// // MainPathAnalysis::localForward(std::back_inserter(main_path), g, weights, s, t);
		// MainPathAnalysis::globalAlpha(std::back_inserter(main_path), g, weights, s, t, alpha);
		// alpha += 1;
		// std::cout << main_path.size() << " " << alpha << std::endl;
	// }
	// while(main_path.size() > 50);

	if(request_parameters.method == RequestParameters::LOCAL)
		MainPathAnalysis::localForward<EventNetwork, MainPathAnalysis::BigInt, std::back_insert_iterator<std::vector<EventNetwork::edge_descriptor>>>(std::back_inserter(main_path), g, weights, s, t);
	if(request_parameters.method == RequestParameters::GLOBAL)
		MainPathAnalysis::global<EventNetwork, MainPathAnalysis::BigInt, std::back_insert_iterator<std::vector<EventNetwork::edge_descriptor>>>(std::back_inserter(main_path), g, weights, s, t);
	if(request_parameters.method == RequestParameters::ALPHA)
	{
		MainPathAnalysis::BigInt alpha = (std::size_t) request_parameters.alpha;
		MainPathAnalysis::globalAlpha(std::back_inserter(main_path), g, weights, s, t, alpha);
	}
	if(request_parameters.method == RequestParameters::ALPHA_LENGTH)
	{
		MainPathAnalysis::BigInt length = request_parameters.main_path_length;
		MainPathAnalysis::globalAlphaLength(std::back_inserter(main_path), g, weights, s, t, length);
	}


	for (std::size_t i = 0; i < main_path.size()-1; i++) {
		if(boost::target(main_path[i],g) != boost::source(main_path[i+1],g))
			std::cout << "VIOLATED MAIN PATH CONSTRAINT" <<  boost::target(main_path[i],g) << " -> "<< boost::source(main_path[i+1],g) << std::endl;
	}

	// remove s and t from main path and from copy of graph
	MainPathAnalysis::remove_edges_containing_source_or_sink(g, s, t, main_path);
	// MainPathAnalysis::remove_source_and_sink_vertex(g, s, t);

	// build json object of main path
	EdgeList main_path_edges;
	for (auto e : main_path) 
		main_path_edges.push_back({ boost::source(e,g), boost::target(e,g) });

	_global_main_path_cache.insert({ compute_hash(category_id, request_parameters), std::move(main_path_edges) });
}


std::size_t ServerDataCache::compute_hash(std::size_t category_id, const RequestParameters& request_parameters) const
{
	std::size_t seed = 0;
	boost::hash_combine(seed, category_id);
	boost::hash_combine(seed, request_parameters.hash());

	return seed;
}


void ServerDataCache::export_event_network_to_file(std::ostream& file, std::size_t category_id, const RequestParameters& request_parameters)
{
	const auto& event_list = get_event_list(category_id, request_parameters);
	const auto& event_network = get_event_network(category_id, request_parameters);

	file << event_list.size() << std::endl;
	file << boost::num_edges(event_network) << std::endl;
	for (auto& event : event_list) {
		file << event.Title << "\t" << (event.Date.tm_year + 1900) << "_" << (event.Date.tm_mon + 1) << "_" << event.Date.tm_mday << std::endl;	
	}

	for (auto e : boost::make_iterator_range(boost::edges(event_network))) {
		file << boost::source(e,event_network) << " " << boost::target(e,event_network) << std::endl;
	}
}
