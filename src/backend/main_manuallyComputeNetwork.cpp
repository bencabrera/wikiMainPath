#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <string>
#include <numeric>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <ctime> 

#include "../../libs/main_path_analysis/src/mainPathAnalysis/checkGraphProperties.hpp"
#include "../../libs/main_path_analysis/src/mainPathAnalysis/edgeWeightGeneration.hpp"
#include "../../libs/main_path_analysis/src/mainPathAnalysis/mpaAlgorithms.hpp"
#include "../../libs/main_path_analysis/src/mainPathAnalysis/preAndPostProcessing.hpp"
#include "../../libs/main_path_analysis/src/mainPathAnalysis/propertyMaps.hpp"

using VertexProperties = boost::property<boost::vertex_name_t, std::string, boost::property<boost::vertex_finish_time_t, std::tm>>;
using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, VertexProperties, boost::property<boost::edge_index_t,std::size_t>, boost::vecS>;

Graph read_graph_from_file(std::istream& input, std::set<std::size_t> forbidden_vertices)
{
	std::size_t n_vertices, n_edges;

	std::string l;
	std::getline(input, l);
	n_vertices = std::stoul(l);
	std::getline(input, l);
	n_edges = std::stoul(l);

	Graph g;

	std::map<std::size_t, Graph::vertex_descriptor> v_map;

	for(std::size_t i = 0; i < n_vertices; i++)
	{
		std::string line;
		std::getline(input, line);
		boost::trim(line);

		if(!line.empty() && forbidden_vertices.find(i) == forbidden_vertices.end())
		{
			auto v = boost::add_vertex(g);
			v_map.insert({ i, v });
			std::vector<std::string> tmp;
			boost::split(tmp, line, boost::is_any_of("\t"));
			boost::trim(tmp[0]);

			boost::put(boost::vertex_name, g, v, tmp[0]);

			std::vector<std::string> date_strs;
			boost::split(date_strs, tmp[1], boost::is_any_of("_"));
			int year = std::stoul(date_strs[0]), month = std::stoul(date_strs[1]), day = std::stoul(date_strs[2]);
			std::tm date{};
			date.tm_year = year-1900;
			date.tm_mon = month-1;
			date.tm_mday = day;

			boost::put(boost::vertex_finish_time, g, v, date);
		}
	}

	for(std::size_t i = 0; i < n_edges; i++)
	{
		std::string line;
		std::getline(input, line);
		boost::trim(line);

		if(!line.empty())
		{
			std::stringstream ss(line);
			std::size_t from, to;
			ss >> from >> to;

			if(forbidden_vertices.find(from) == forbidden_vertices.end() && forbidden_vertices.find(to) == forbidden_vertices.end())
				boost::add_edge(v_map.at(from), v_map.at(to), g);
		}
	}

	return g;
}

class VertexLabelWriter {
	public:
		VertexLabelWriter(Graph& g, std::vector<std::vector<Graph::edge_descriptor>> main_paths = std::vector<std::vector<Graph::edge_descriptor>>()) 
			:_g(g),
			main_path_colors({ "blue", "green", "red" })
		{
			for (auto main_path : main_paths) {
				std::set<Graph::vertex_descriptor> vertices_in_path;
				for (auto e : main_path) {
					vertices_in_path.insert(boost::source(e,g));	
					vertices_in_path.insert(boost::target(e,g));	
				}
				_main_path_vertex_sets.push_back(vertices_in_path);
			}	
		}

		template <class VertexOrEdge>
			void operator()(std::ostream& out, const VertexOrEdge& v) const {
				const std::size_t MAX_LENGTH = 50;
				std::string lab = boost::get(boost::vertex_name, _g, v);
				if(lab.length() > MAX_LENGTH+5)
					lab = lab.substr(0, MAX_LENGTH) + "[...]";
				out << "[";
				out << "label=\"" << lab << "\" ";

				std::size_t i = 0;
				for (auto mp : _main_path_vertex_sets) 
				{
					if(mp.find(v) != mp.end())
					{
						out << "color=\"" << main_path_colors[i] << "\" ";
					}
					i++;
				}
				// out << "shape=point ";

				out << "fontsize=70 ";
				out << "]";
			}

	private:
		Graph& _g;

		std::vector<std::set<Graph::vertex_descriptor>> _main_path_vertex_sets;
		const std::vector<std::string> main_path_colors;
};

class EdgeLabelWriter {
	public:
		EdgeLabelWriter(Graph& g, std::vector<std::vector<Graph::edge_descriptor>> main_paths) 
			:_g(g),
			main_path_colors({ "blue", "green", "red" })
		{
			for (auto mp : main_paths) {
				_main_path_sets.push_back(std::set<Graph::edge_descriptor>(mp.begin(), mp.end()));
			}
		}


		template <class VertexOrEdge>
			void operator()(std::ostream& out, const VertexOrEdge& e) const {
				out << "[";
				out << "fontsize=40 ";

				std::size_t i = 0;
				bool found = false;
				for (auto mp : _main_path_sets) 
				{
					if(mp.find(e) != mp.end())
					{
						out << "color=\"" << main_path_colors[i] << "\" ";
						found = true;
					}
					i++;
				}

				if(found)
					out << "penwidth=\"20\" ";
				else
					out << "penwidth=\"15\" ";
				out << "]";
			}

	private:
		Graph& _g;
		std::vector<std::set<Graph::edge_descriptor>> _main_path_sets;
		const std::vector<std::string> main_path_colors;
};


int main(int argc, char *argv[])
{
	std::ifstream input_file(argv[1]);	

	std::set<std::size_t> forbidden_vertices;
	if(argc > 2)
	{
		std::ifstream forbidden_file(argv[2]);	
		std::size_t i_forbidden;
		while(!forbidden_file.eof())
		{
			forbidden_file >> i_forbidden;	
			forbidden_vertices.insert(i_forbidden);
		}
	}

	auto g = read_graph_from_file(input_file, forbidden_vertices);

	std::vector<Graph::vertex_descriptor> ordered_vertices(boost::num_vertices(g));
	std::iota(ordered_vertices.begin(), ordered_vertices.end(), 0);
	std::sort(ordered_vertices.begin(), ordered_vertices.end(), [&g](const Graph::vertex_descriptor& v1, const Graph::vertex_descriptor& v2)
	{
		return boost::in_degree(v1,g) + boost::out_degree(v1,g) < boost::in_degree(v2,g) + boost::out_degree(v2,g);
	});

	for (auto v : ordered_vertices) {
		std::cout << boost::get(boost::vertex_name, g, v) << std::endl;
	}

	std::cout << boost::num_vertices(g) << " " << boost::num_edges(g) << std::endl;

	MainPathAnalysis::set_increasing_edge_index(g);

	// add s and t vertex
	Graph::vertex_descriptor s, t;
	MainPathAnalysis::add_source_and_sink_vertex<Graph>(g, s, t);

	// compute spc weights
	// auto weights = MainPathAnalysis::generate_spc_weights(g, s, t);
	auto weights = MainPathAnalysis::generate_spc_weights_big_int(g, s, t);

	// compute global main path
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

	std::vector<Graph::edge_descriptor> main_path_local, main_path_global, main_path_alpha;
	MainPathAnalysis::localForward<Graph, MainPathAnalysis::BigInt, std::back_insert_iterator<std::vector<Graph::edge_descriptor>>>(std::back_inserter(main_path_local), g, weights, s, t);
	// MainPathAnalysis::global(std::back_inserter(main_path_global), g, weights, s, t);

	if(argc > 5)
	{
		main_path_local.clear();

		std::cout << "Alpha Main Path" << std::endl;
		std::size_t max_length = std::stoul(argv[5]);

		MainPathAnalysis::BigInt upper_alpha = 1;
		MainPathAnalysis::BigInt lower_alpha = 0;

		MainPathAnalysis::globalAlpha(std::back_inserter(main_path_local), g, weights, s, t, lower_alpha);

		// multiply by 2 until bigger
		while(main_path_local.size() > max_length)
		{
			main_path_local.clear();
			MainPathAnalysis::globalAlpha(std::back_inserter(main_path_local), g, weights, s, t, upper_alpha);
			upper_alpha *= 2;
		}

		std::cout << "UPPER ALPHA " << upper_alpha << std::endl;

		// binary search
		bool found = false;
		while(!found)
		{
			std::cout << "loop" << std::endl;
			MainPathAnalysis::BigInt middle_alpha = (upper_alpha + lower_alpha) / 2;

			std::cout <<  lower_alpha << std::endl;
			std::cout <<  middle_alpha << std::endl;
			std::cout <<  upper_alpha << std::endl;

			main_path_local.clear();
			MainPathAnalysis::globalAlpha(std::back_inserter(main_path_local), g, weights, s, t, middle_alpha);

			std::cout << "SIZE: " << main_path_local.size() << std::endl;

			if(middle_alpha == upper_alpha || middle_alpha == lower_alpha)
				break;

			if(main_path_local.size() > max_length)
				lower_alpha = middle_alpha;
			else
				upper_alpha = middle_alpha;

			if(main_path_local.size() == max_length)
				found = true;

			std::cout << std::endl;


		}	

	}

	// remove s and t from main path and from copy of graph
	// MainPathAnalysis::remove_edges_containing_source_or_sink(g, s, t, main_path_global);
	MainPathAnalysis::remove_edges_containing_source_or_sink(g, s, t, main_path_local);
	// MainPathAnalysis::remove_edges_containing_source_or_sink(g, s, t, main_path_alpha);
	MainPathAnalysis::remove_source_and_sink_vertex(g, s, t);

	// build json object of main path
	// EdgeList main_path_edges;
	// for (auto e : main_path) 
		// main_path_edges.push_back({ boost::source(e,g), boost::target(e,g) });

	// _global_main_path_cache.insert({ category_id, std::move(main_path_edges) });


	std::cout << "main_path_local: " << main_path_local.size() << std::endl;
	std::cout << "main_path_global: " << main_path_global.size() << std::endl;
	std::cout << "main_path_alpha: " << main_path_alpha.size() << std::endl;

	if(argc > 3)
	{
		std::ofstream output_file(argv[3]);
		VertexLabelWriter vertex_label_writer(g, { main_path_local });
		EdgeLabelWriter edge_label_writer(g, { main_path_local });
		boost::write_graphviz(output_file, g, vertex_label_writer, edge_label_writer);
	}

	if(argc > 4)
	{
		std::ofstream output_file(argv[4]);

		const auto& mp = main_path_local;
		Graph mp_graph(mp.size() + 1);
		std::size_t i_vertex = 0;
		for (const auto& e : mp) {
			boost::add_edge(i_vertex, i_vertex+1, mp_graph);	
			boost::put(boost::vertex_name, mp_graph, i_vertex, boost::get(boost::vertex_name, g, boost::source(e,g)));
			boost::put(boost::vertex_finish_time, mp_graph, i_vertex, boost::get(boost::vertex_finish_time, g, boost::source(e,g)));
			i_vertex++;
		}
		boost::put(boost::vertex_name, mp_graph, i_vertex, boost::get(boost::vertex_name, g, boost::target(mp.back(),g)));
		boost::put(boost::vertex_finish_time, mp_graph, i_vertex, boost::get(boost::vertex_finish_time, g, boost::target(mp.back(),g)));

		VertexLabelWriter vertex_label_writer(mp_graph);
		boost::write_graphviz(output_file, mp_graph, vertex_label_writer);
	}

	return 0;
}
