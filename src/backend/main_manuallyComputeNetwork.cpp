#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <ctime> 

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


int main(int argc, char *argv[])
{
	std::ifstream input_file(argv[1]);	
	auto g = read_graph_from_file(input_file);

	std::cout << boost::num_vertices(g) << " " << boost::num_edges(g) << std::endl;

	if(argc > 2)
	{
		std::ofstream output_file(argv[2]);
		VertexLabelWriter vertex_label_writer(g);
		boost::write_graphviz(output_file, g, vertex_label_writer);
	}

	return 0;
}
