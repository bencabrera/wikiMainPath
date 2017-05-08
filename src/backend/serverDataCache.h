#pragma once

#include "../core/graph.h"
#include "../core/wikiDataCache.h"

#include <Poco/Mutex.h>
#include <map>
#include <list>
#include <array>

class ServerDataCache {
	public:
		struct Event {
			std::string Title;			
			std::tm Date;			
		};

		using EventList = std::vector<Event>;
		using EdgeList = std::vector<std::pair<std::size_t,std::size_t>>;

		static constexpr std::size_t MAX_CACHE_SIZE = 10;
		static constexpr std::size_t N_MUTEX = 100;

		ServerDataCache(const WikiMainPath::WikiDataCache& wiki_data_cache);

		const std::vector<std::size_t>& get_article_list(std::size_t category_id);
		const ArticleGraph& get_event_network(std::size_t category_id);
		const EventList& get_event_list(std::size_t category_id);
		const std::vector<double>& get_network_positions(std::size_t category_id);
		const EdgeList& get_global_main_path(std::size_t category_id);

	private:

		static std::vector<double> compute_x_positions(const EventList& event_list);

		void compute_article_list(std::size_t category_id);
		std::map<std::size_t, std::vector<std::size_t>> _article_list_cache;
		std::list<std::size_t> _article_list_priority_list;

		void compute_event_network(std::size_t category_id);
		std::map<std::size_t, ArticleGraph> _event_network_cache;
		std::list<std::size_t> _event_network_priority_list;

		void compute_event_list(std::size_t category_id);
		std::map<std::size_t, EventList> _event_list_cache;
		std::list<std::size_t> _event_list_priority_list;

		void compute_network_positions(std::size_t category_id);
		std::map<std::size_t, std::vector<double>> _network_positions_cache;
		std::list<std::size_t> _network_positions_priority_list;

		void compute_global_main_path(std::size_t category_id);
		std::map<std::size_t, EdgeList> _global_main_path_cache;
		std::list<std::size_t> _global_main_path_priority_list;



		const WikiMainPath::WikiDataCache& _wiki_data_cache;

		const std::vector<std::string>& _article_titles;
		const std::vector<std::string>& _category_titles;
		const std::vector<std::vector<Date>>& _article_dates;
		const std::vector<std::vector<std::size_t>>& _category_has_article;
		const std::vector<std::vector<std::size_t>>& _article_network;
		const CategoryHirachyGraph& _category_hirachy_graph;
		// const std::vector<std::size_t>& _event_indices;
		// ArticleGraph _event_network;

		std::array<Poco::Mutex, N_MUTEX> _mutices;

	public:
		template<typename FType>
		using Filters = std::vector<std::function<FType>>;

		Filters<bool(const std::string&, const std::vector<Date>&)> article_filters;
};
