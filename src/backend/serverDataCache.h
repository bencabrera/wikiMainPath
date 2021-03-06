#pragma once

#include "../core/graph.h"
#include "../core/wikiDataCache.h"
#include "requestParameters.h"

#include <Poco/Mutex.h>
#include <map>
#include <list>
#include <array>

class ServerDataCache {
	public:
		struct Event {
			std::string Title;			
			std::tm Date;
			std::size_t ArticleId;
		};

		using EventList = std::vector<Event>;
		using EdgeList = std::vector<std::pair<std::size_t,std::size_t>>;

		static constexpr std::size_t MAX_CACHE_SIZE = 10;
		static constexpr std::size_t N_MUTEX = 100;

		ServerDataCache(const WikiMainPath::WikiDataCache& wiki_data_cache);

		const std::vector<std::size_t>& get_article_list(std::size_t category_id, const RequestParameters& request_parameters);
		const EventList& get_event_list(std::size_t category_id, const RequestParameters& request_parameters);

		const EventNetwork& get_event_network(std::size_t category_id, const RequestParameters& request_parameters);
		const std::vector<double>& get_network_positions(std::size_t category_id, const RequestParameters& request_parameters);
		const EdgeList& get_global_main_path(std::size_t category_id, const RequestParameters& request_parameters);

		void export_event_network_to_file(std::ostream& file, std::size_t category_id, const RequestParameters& request_parameters);

	private:

		static std::vector<double> compute_x_positions(const EventList& event_list);

		void compute_article_list(std::size_t category_id, const RequestParameters& request_parameters);
		std::map<std::size_t, std::vector<std::size_t>> _article_list_cache;
		std::list<std::size_t> _article_list_priority_list;

		void compute_event_network(std::size_t category_id, const RequestParameters& request_parameters);
		std::map<std::size_t, EventNetwork> _event_network_cache;
		std::list<std::size_t> _event_network_priority_list;

		void compute_event_list(std::size_t category_id, const RequestParameters& request_parameters);
		std::map<std::size_t, EventList> _event_list_cache;
		std::list<std::size_t> _event_list_priority_list;

		void compute_network_positions(std::size_t category_id, const RequestParameters& request_parameters);
		std::map<std::size_t, std::vector<double>> _network_positions_cache;
		std::list<std::size_t> _network_positions_priority_list;

		void compute_global_main_path(std::size_t category_id, const RequestParameters& request_parameters);
		std::map<std::size_t, EdgeList> _global_main_path_cache;
		std::list<std::size_t> _global_main_path_priority_list;


		std::size_t compute_hash(std::size_t category_id, const RequestParameters& request_parameters) const;

		const WikiMainPath::WikiDataCache& _wiki_data_cache;

		const std::vector<std::string>& _article_titles;
		const std::vector<std::string>& _category_titles;
		const std::vector<std::vector<Date>>& _article_dates;
		const std::vector<std::vector<std::size_t>>& _category_has_article;
		const std::vector<std::vector<std::size_t>>& _article_network;
		const CategoryHirachyGraph& _category_hirachy_graph;

		std::array<Poco::Mutex, N_MUTEX> _mutices;
};
