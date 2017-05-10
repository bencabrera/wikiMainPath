#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <ctime> 

#include "../../libs/main_path_analysis/src/mainPathAnalysis/checkGraphProperties.hpp"
#include "../../libs/main_path_analysis/src/mainPathAnalysis/edgeWeightGeneration.hpp"
#include "../../libs/main_path_analysis/src/mainPathAnalysis/mpaAlgorithms.hpp"
#include "../../libs/main_path_analysis/src/mainPathAnalysis/preAndPostProcessing.hpp"

using VertexProperties = boost::property<boost::vertex_name_t, std::string, boost::property<boost::vertex_finish_time_t, std::tm>>;
using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, VertexProperties, boost::property<boost::edge_index_t,std::size_t>, boost::vecS>;

Graph read_graph_from_file(std::istream& input)
{
	std::size_t n_vertices, n_edges;

	std::string l;
	std::getline(input, l);
	n_vertices = std::stoul(l);
	std::getline(input, l);
	n_edges = std::stoul(l);

	Graph g(n_vertices);

	for(std::size_t i = 0; i < n_vertices; i++)
	{
		std::string line;
		std::getline(input, line);
		boost::trim(line);

		if(!line.empty())
		{
			std::vector<std::string> tmp;
			boost::split(tmp, line, boost::is_any_of("\t"));
			boost::trim(tmp[0]);

			boost::put(boost::vertex_name, g, i, tmp[0]);

			std::vector<std::string> date_strs;
			boost::split(date_strs, tmp[1], boost::is_any_of("_"));
			int year = std::stoul(date_strs[0]), month = std::stoul(date_strs[1]), day = std::stoul(date_strs[2]);
			std::tm date{};
			date.tm_year = year-1900;
			date.tm_mon = month-1;
			date.tm_mday = day;

			boost::put(boost::vertex_finish_time, g, i, date);
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
			boost::add_edge(from, to, g);
		}
	}

	return g;
}

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

class EdgeLabelWriter {
	public:
		EdgeLabelWriter(Graph& g, MainPathAnalysis::DoubleEdgePropertyMap<Graph>& edge_weights, std::vector<std::vector<Graph::edge_descriptor>> main_paths) 
			:_g(g),
			_edge_weights(edge_weights),
			main_path_colors({ "blue", "green", "red" })
		{
			for (auto mp : main_paths) {
				_main_path_sets.push_back(std::set<Graph::edge_descriptor>(mp.begin(), mp.end()));
			}
		}


		template <class VertexOrEdge>
			void operator()(std::ostream& out, const VertexOrEdge& e) const {
				out << "[";
				out << "label=\"" << _edge_weights[e] << "\" ";

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
					out << "penwidth=\"8\" ";
				out << "]";
			}

	private:
		Graph& _g;
		MainPathAnalysis::DoubleEdgePropertyMap<Graph>& _edge_weights;
		std::vector<std::set<Graph::edge_descriptor>> _main_path_sets;
		const std::vector<std::string> main_path_colors;
};


int main(int argc, char *argv[])
{
	std::ifstream input_file(argv[1]);	
	auto g = read_graph_from_file(input_file);

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
	MainPathAnalysis::localForward(std::back_inserter(main_path_local), g, weights, s, t);
	MainPathAnalysis::global(std::back_inserter(main_path_global), g, weights, s, t);
	MainPathAnalysis::globalAlpha(std::back_inserter(main_path_alpha), g, weights, s, t, 10);

	// remove s and t from main path and from copy of graph
	// MainPathAnalysis::remove_edges_containing_source_or_sink(g, s, t, main_path);
	// MainPathAnalysis::remove_source_and_sink_vertex(g, s, t);

	// build json object of main path
	// EdgeList main_path_edges;
	// for (auto e : main_path) 
		// main_path_edges.push_back({ boost::source(e,g), boost::target(e,g) });

	// _global_main_path_cache.insert({ category_id, std::move(main_path_edges) });


	std::cout << "main_path_local: " << main_path_local.size() << std::endl;
	std::cout << "main_path_global: " << main_path_global.size() << std::endl;
	std::cout << "main_path_alpha: " << main_path_alpha.size() << std::endl;

	if(argc > 2)
	{
		std::ofstream output_file(argv[2]);
		VertexLabelWriter vertex_label_writer(g);
		EdgeLabelWriter edge_label_writer(g, weights, { main_path_local, main_path_global, main_path_alpha });
		boost::write_graphviz(output_file, g, vertex_label_writer, edge_label_writer);
	}

	return 0;
}
