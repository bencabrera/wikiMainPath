#include "../core/wikiDataCache.h"
#include "serverDataCache.h"
#include "filters.h"
#include <boost/filesystem.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/property_map/vector_property_map.hpp>
#include <list>

#include "../../libs/shared/cpp/stepTimer.hpp"

using VertexProperties = boost::property<boost::vertex_name_t, std::string, boost::property<boost::vertex_local_index_t,std::size_t>>;
using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, VertexProperties, boost::property<boost::edge_index_t,std::size_t>, boost::vecS>;


class VertexLabelWriter {
	public:
		VertexLabelWriter(Graph& g) 
			:_g(g)
		{}

		template <class VertexOrEdge>
			void operator()(std::ostream& out, const VertexOrEdge& v) const {
				out << "[label=\"" << boost::get(boost::vertex_name, _g, v) << "\"]";
			}

	private:
		Graph& _g;
};

Graph compute_category_subhirachy(const std::vector<std::vector<std::size_t>>& category_has_article, std::size_t i_category, const CategoryHirachyGraph& graph, const std::vector<std::string>& category_titles)
{
	class CategoryTree : public boost::default_dfs_visitor
	{
		public:
			CategoryTree(const std::vector<std::vector<std::size_t>>& category_has_article, std::set<std::size_t>& s, std::set<std::size_t>& s2)
				:_category_has_article(category_has_article),
				articles_in_category(s),
				vertices_in_category_graph(s2)
		{}

			void discover_vertex(CategoryHirachyGraph::vertex_descriptor v, const CategoryHirachyGraph& g)
			{
				articles_in_category.insert(_category_has_article[v].begin(), _category_has_article[v].end());	
				vertices_in_category_graph.insert(v);
			}

			const std::vector<std::vector<std::size_t>>& _category_has_article;
			std::set<std::size_t>& articles_in_category;
			std::set<std::size_t>& vertices_in_category_graph;
	};

	std::set<std::size_t> articles_in_category;
	std::set<std::size_t> vertices_in_category_graph;
	CategoryTree tree(category_has_article, articles_in_category, vertices_in_category_graph);
	boost::vector_property_map<boost::default_color_type> color_map;
	boost::depth_first_visit(graph, i_category, tree, color_map);

	std::map<CategoryHirachyGraph::vertex_descriptor, std::size_t> v_map;
	std::size_t i = 0;
	Graph subgraph(vertices_in_category_graph.size());
	for (const auto& v : vertices_in_category_graph) 
	{
		v_map.insert({ v, i });	
		std::string title = category_titles[v];
		boost::put(boost::vertex_name, subgraph, i, title);
		boost::put(boost::vertex_local_index, subgraph, i, v);
		i++;
	}

	std::cout << "tree.size: " << vertices_in_category_graph.size() << std::endl;

	for (const auto& v : vertices_in_category_graph) {
		for (auto e : boost::make_iterator_range(boost::out_edges(v,graph))) {
			if(vertices_in_category_graph.find(boost::target(e,graph)) != vertices_in_category_graph.end())
				boost::add_edge(v_map.at(v), v_map.at(boost::target(e,graph)), subgraph);
		}
	}

	return subgraph;
}

void write_category_subhirachy(std::ostream& ostr, Graph& category_subhirachy)
{
	VertexLabelWriter vertex_label_writer(category_subhirachy);
	boost::write_graphviz(ostr, category_subhirachy, vertex_label_writer);
}

int main(int argc, char** argv)
{
	using namespace WikiMainPath;

	auto data_path = boost::filesystem::path(argv[1]);

	if(!boost::filesystem::is_directory(data_path))
	{
		std::cerr << "Specified folder is non-existent." << std::endl;
		return 1;
	}


	std::cout << "Scanning path " << data_path << " for data files." << std::endl;
	Shared::StepTimer timer;

	timer.start_timing_step("global","Reading in data files", &std::cout);

	WikiDataCache data(data_path.string());
	timer.start_timing_step("read_article_titles","Reading in 'article_titles'", &std::cout);
	const auto& article_titles = data.article_titles();
	std::cout << article_titles.size() << std::endl;
	timer.stop_timing_step("read_article_titles",&std::cout);
	timer.start_timing_step("read_category_titles","Reading in 'category_titles'", &std::cout);
	const auto& category_titles = data.category_titles();
	std::cout << category_titles.size() << std::endl;
	timer.stop_timing_step("read_category_titles",&std::cout);
	timer.start_timing_step("read_category_has_article","Reading in 'category_has_article'", &std::cout);
	const auto& category_has_article = data.category_has_article();
	std::cout << category_has_article.size() << std::endl;
	timer.stop_timing_step("read_category_has_article",&std::cout);
	timer.start_timing_step("read_article_dates","Reading in 'article_dates'", &std::cout);
	data.article_dates();
	timer.stop_timing_step("read_article_dates",&std::cout);
	timer.start_timing_step("read_article_network","Reading in 'article_network'", &std::cout);
	data.article_network();
	timer.stop_timing_step("read_article_network",&std::cout);
	timer.start_timing_step("read_category_hirachy_graph","Reading in 'category_hirachy_graph'", &std::cout);
	const auto& category_hirachy_graph = data.category_hirachy_graph();
	timer.stop_timing_step("read_category_hirachy_graph",&std::cout);
	std::cout << boost::num_vertices(category_hirachy_graph) << std::endl;

	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "IO Timings: " << std::endl;
	timer.print_timings(std::cout);
	std::cout << "---------------------------------------------------" << std::endl;



	Shared::StepTimer timer_server;

	ServerDataCache _server_data_cache(data);

	// _server_data_cache.article_filters.push_back(articles_without_certain_dates({ "Birth", "Death", "Released", "Recorded", "First aired", "Last aired", "Term started", "Term ended" }));
	// _server_data_cache.event_filters.push_back(events_in_date_range(create_date_range(1750,1,1,1820,1,1)));

	// const std::size_t category_id = 722580; // friendly fire incidents
	// const std::size_t category_id = 719007; // french revolution
	// const std::size_t category_id = 654622; // events of french revolution
	const std::size_t category_id = 1564179; // world war II
	// const std::size_t category_id = 1409141; // thirty years war
	// const std::size_t category_id = 719009; // french revolution films 
	// const std::size_t category_id = 627973; // edward snowden
	// const std::size_t category_id = 1033233; // norman conquest
	// const std::size_t category_id = 1452526; // ukrainian crisis


	std::cout << "CATEGORY: " << category_titles[category_id] << std::endl;

	// auto battle_it = std::lower_bound(article_titles.begin(), article_titles.end(), "Battle of Hastings");
	// std::cout << "Battle of Hastings" << std::endl;
	// if(battle_it != article_titles.end())
	// {
		// auto article_id = battle_it - article_titles.begin();
		// std::cout << article_id << std::endl;
		// std::cout << article_titles[article_id-1] << std::endl;
		// std::cout << article_titles[article_id] << std::endl;
		// std::cout << article_titles[article_id+1] << std::endl;
	// }
	// else
		// std::cout << "Not found" << std::endl;


	// build category subhirachy graph
	// auto category_subhirachy = compute_category_subhirachy(category_has_article, category_id, category_hirachy_graph, category_titles);

	// std::ofstream hirachy_file("/home/cabrera/Schreibtisch/hirachy.dot");
	// write_category_subhirachy(hirachy_file, category_subhirachy);


	// // see where a particular article comes from in category_hirachy
	// std::string article_name = "Battle of Pensacola (1814)";
	// auto battle_it2 = std::lower_bound(article_titles.begin(), article_titles.end(), article_name);
	// std::size_t where_article_id = battle_it2 - article_titles.begin();

	// std::cout << article_titles[where_article_id] << std::endl;

	// // find category in which this article lives
	// bool found_containing_category = false;
	// std::size_t containing_category = 0;
	// Graph::vertex_descriptor cat_vertex;
	// for (auto v : boost::make_iterator_range(boost::vertices(category_subhirachy))) {
		// auto global_cat_id = boost::get(boost::vertex_local_index, category_subhirachy, v);	
		// for (std::size_t i : category_has_article[global_cat_id]) {
			// if(where_article_id == i)
			// {
				// containing_category = global_cat_id;
				// found_containing_category = true;
				// cat_vertex = v;
				// break;
			// }

			// if(found_containing_category) 
				// break;
		// }
	// }

	// if(found_containing_category)
		// std::cout << "CONTAINING CATEGORY: " << category_titles[containing_category] << std::endl;
	// else
		// std::cout << "DID NOT FIND CONTAINING CATEGORY: " << std::endl;

	// std::list<std::string> categories_that_lead_here;
	// while(boost::in_degree(cat_vertex, category_subhirachy) != 0)
	// {
		// categories_that_lead_here.push_front(category_titles[boost::get(boost::vertex_local_index, category_subhirachy, cat_vertex)]);
		// cat_vertex = boost::source(*boost::in_edges(cat_vertex, category_subhirachy).first, category_subhirachy);
	// }
	// categories_that_lead_here.push_front(category_titles[boost::get(boost::vertex_local_index, category_subhirachy, cat_vertex)]);

	// for (auto c : categories_that_lead_here) {
		// std::cout << c << std::endl;	
	// }

	std::cout << "-----------------" << std::endl;


	RequestParameters request_parameters{ true, 1939, 1945, RequestParameters::ALPHA, 179.3, true, false, "" };

	std::ofstream network_file("/home/ace7k3/Desktop/network.txt");
	_server_data_cache.export_event_network_to_file(network_file, category_id, request_parameters);

	std::cout << "Wrote network file" << std::endl;

	// for (auto v : boost::make_iterator_range(boost::vertices(category_subhirachy))) {
		// auto global_cat_id = boost::get(boost::vertex_local_index, category_subhirachy, v);	
		// auto category_title = category_titles[global_cat_id];

		// std::cout << "--- " << category_title << " ---" << std::endl;
		// for (auto i_article : category_has_article[global_cat_id]) 
			// std::cout << article_titles[i_article] << std::endl;	
		// std::cout << "--- " << " ---" << std::endl;
	// }


	// timer_server.start_timing_step("build_article_list", "Build article list", &std::cout);
	// const auto& article_list = _server_data_cache.get_article_list(category_id, request_parameters);
	// std::cout << article_list.size() << std::endl;
	// // timer_server.stop_timing_step("build_article_list", &std::cout);

	// // timer_server.start_timing_step("build_event_list", "Build event list", &std::cout);
	// const auto& event_list = _server_data_cache.get_event_list(category_id, request_parameters);
	// std::cout << event_list.size() << std::endl;
	// // timer_server.stop_timing_step("build_event_list", &std::cout);

	// // for (auto event : event_list) {
	// // std::cout << std::setw(80) << event.Title << " " << to_iso_string(event.Date) << std::endl;	
	// // }

	// // timer_server.start_timing_step("build_event_network", "Build event network", &std::cout);
	// const auto& event_network = _server_data_cache.get_event_network(category_id, request_parameters);
	// std::cout << boost::num_vertices(event_network) << std::endl;
	// // timer_server.stop_timing_step("build_event_network", &std::cout);

	// // // std::ofstream graphviz_file("/home/cabrera/Schreibtisch/test.dot");
	// // // boost::write_graphviz(graphviz_file, event_network);

	// // timer_server.start_timing_step("build_positions", "Build positions", &std::cout);
	// _server_data_cache.get_network_positions(category_id, request_parameters);
	// // // std::cout << positions.size() << std::endl;
	// // // std::cout << "positions: " << std::endl;
	// // // for (auto pos : positions) {
	// // // std::cout << pos << std::endl;
	// // // }
	// // timer_server.stop_timing_step("build_positions", &std::cout);


	// // timer_server.start_timing_step("build_main_path", "Build main path", &std::cout);
	// const auto& main_path = _server_data_cache.get_global_main_path(category_id, request_parameters);
	// std::cout << main_path.size() << std::endl;
	// // timer_server.stop_timing_step("build_main_path", &std::cout);

	timer_server.print_timings(std::cout);



	return 0;
}
