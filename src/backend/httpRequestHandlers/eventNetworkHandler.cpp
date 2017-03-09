#include "eventNetworkHandler.h"

#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <Poco/JSON/Object.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/URI.h>

#include <algorithm>

#include "../graphDrawing/randomGraphDrawing.h"

namespace WikiMainPath {

	EventNetworkHandler::EventNetworkHandler(const WikiDataCache& wiki_data_cache)
		:_data(wiki_data_cache)
	{}

	void EventNetworkHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
	{
		using namespace Poco::JSON;

		response.setChunkedTransferEncoding(true);
		response.setContentType("text/json");
		response.set("Access-Control-Allow-Origin", "*");

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

		try {
			if(category_id >= _data.category_has_article().size())
			{
				response.setStatus("400"); // = Bad Request

				Object root;
				root.set("error", "GET-Parameter category-id was larger than the sizes of _category_has_article.");

				root.stringify(response.send(), 4); 

				return;
			}
		}
		catch(std::logic_error e)
		{
			std::cout << "Hello_logic" << e.what() << std::endl;
		}

		const auto& article_titles = _data.article_titles();
		const auto& article_dates = _data.article_dates();
		const auto& category_has_article = _data.category_has_article();
		const auto& event_indices = _data.event_indices();
		auto event_network = _data.event_network();
		// const auto& article_network = _data.article_network();

		// TODO: make event indices one larger
		std::vector<std::size_t> events_in_category;	
		for (auto article_id : category_has_article[category_id]) {
			for(std::size_t idx = event_indices[article_id]; idx < event_indices[article_id+1]; idx++)
				events_in_category.push_back(idx);
		}

		EventNetwork subgraph = event_network.create_subgraph();
		for (auto i : events_in_category) 
			boost::add_vertex(i, subgraph);	

		// build list of articles in category
		Array events_array;
		for (auto v : boost::make_iterator_range(boost::vertices(subgraph))) {

			auto global_event_id = subgraph.local_to_global(v);

			auto article_it = std::lower_bound(event_indices.begin(), event_indices.end(), global_event_id);	
			std::size_t article_id = article_it - event_indices.begin();
			if(event_indices[article_id] != global_event_id)
				article_id -= 1;
			std::size_t date_id = global_event_id - event_indices[article_id];

			std::string article_title = boost::to_upper_copy(article_dates[article_id][date_id].Description) + ": " + article_titles[article_id];

			Object el;
			el.set("title", article_title);
			el.set("id", global_event_id);
			el.set("date", to_iso_string(article_dates[article_id][date_id].Begin));

			events_array.add(el);
		}

		Array links_array;
		for (auto e : boost::make_iterator_range(boost::edges(subgraph))) {

			Array link;
			link.set(0, boost::source(e, subgraph));
			link.set(1, boost::target(e, subgraph));

			links_array.add(link);
		}

		auto positions = random_graph_drawing(subgraph);
		Array positions_array;
		for (auto pos : positions) 
			positions_array.add(pos);

		Object root;
		root.set("events", events_array);
		root.set("links", links_array);
		root.set("positions", positions_array);

		root.stringify(response.send(), 4); 
	}

}
