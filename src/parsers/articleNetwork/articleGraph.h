#include <string>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

typedef boost::property< boost::vertex_name_t, std::string > StringVertexProperties;

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, StringVertexProperties, boost::no_property, boost::vecS> UndirectedArticleGraph;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, StringVertexProperties, boost::no_property, boost::vecS> DirectedArticleGraph;
