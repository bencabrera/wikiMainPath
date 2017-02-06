#include "mpaHandler.h"

#include <boost/range/iterator_range.hpp>

#include <Poco/JSON/Object.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/URI.h>

#include "../../../libs/main_path_analysis/src/mainPathAnalysis/checkGraphProperties.hpp"
#include "../../../libs/main_path_analysis/src/mainPathAnalysis/edgeWeightGeneration.hpp"
#include "../../../libs/main_path_analysis/src/mainPathAnalysis/mpaAlgorithms.hpp"
#include "../../../libs/main_path_analysis/src/mainPathAnalysis/preAndPostProcessing.hpp"

namespace WikiMainPath {

	MpaHandler::MpaHandler(const std::vector<std::string>& articles, const std::vector<std::vector<std::size_t>>& category_has_article, ServerData::Subgraph& article_network)
		:_articles(articles),
		_category_has_article(category_has_article),
		_article_network(article_network)
	{}

	void MpaHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
	{
		using namespace Poco::JSON;

		response.setChunkedTransferEncoding(true);
		response.setContentType("text/json");

		// get parameters
		Poco::URI uri(request.getURI());
		auto parameters = uri.getQueryParameters();

		if(parameters.size() != 1 || parameters[0].first != "category-id")
		{
			response.setStatus("400"); // = Bad Request

			Object root;
			root.set("error", "GET-Parameter category-id not specified or too many parameters.");

			root.stringify(response.send(), 4); 

			return;
		}

		std::string category_id_str = parameters[0].second;
		std::size_t category_id = std::stoul(category_id_str);

		if(category_id >= _category_has_article.size())
		{
			response.setStatus("400"); // = Bad Request

			Object root;
			root.set("error", "GET-Parameter category-id was larger than the sizes of _category_has_article.");

			root.stringify(response.send(), 4); 

			return;
		}


		auto subgraph = _article_network.create_subgraph(_category_has_article[category_id].begin(), _category_has_article[category_id].end());


		// build list of articles in category
		Array articles_array;
		for (auto v : boost::make_iterator_range(boost::vertices(subgraph))) {

			auto global_article_id = subgraph.local_to_global(v);
			auto& article_name = _articles[global_article_id];

			Object el;
			el.set("title", article_name);
			el.set("id", global_article_id);

			articles_array.add(el);
		}

		Array links_array;
		for (auto e : boost::make_iterator_range(boost::edges(subgraph))) {

			Array link;
			link.set(0, boost::source(e, subgraph));
			link.set(0, boost::target(e, subgraph));

			links_array.add(link);
		}



		// copy subgraph into a new object so that we can add s and t
		ServerData::Graph g;
		boost::copy_graph(subgraph, g);
		MainPathAnalysis::set_increasing_edge_index(g);

		// add s and t vertex
		ServerData::Graph::vertex_descriptor s, t;
		MainPathAnalysis::add_source_and_sink_vertex<ServerData::Graph>(g, s, t);

		// compute spc weights
		auto weights = MainPathAnalysis::generate_spc_weights(g, s, t);

		// compute global main path
		std::vector<ServerData::Graph::edge_descriptor> main_path;
		MainPathAnalysis::global(std::back_inserter(main_path), g, weights, s, t);
		
		// remove s and t from main path and from copy of graph
		MainPathAnalysis::remove_edges_containing_source_or_sink(g, s, t, main_path);
		MainPathAnalysis::remove_source_and_sink_vertex(g, s, t);

		// build json object of main path
		Array main_path_edges;
		for (auto e : main_path) 
			main_path_edges.add(boost::get(boost::edge_index, g, e));
		
		Object root;
		root.set("articles", articles_array);
		root.set("links", links_array);
		root.set("main_path_edges", main_path_edges);
		root.stringify(response.send(), 4); 
	}

}
