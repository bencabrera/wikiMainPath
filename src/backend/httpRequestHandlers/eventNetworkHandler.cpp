#include "eventNetworkHandler.h"

#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <Poco/JSON/Object.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/URI.h>

#include <algorithm>

#include "../graphDrawing/randomGraphDrawing.h"
#include "../../../libs/shared/cpp/stepTimer.hpp"

namespace WikiMainPath {

	EventNetworkHandler::EventNetworkHandler(const WikiDataCache& wiki_data_cache, ServerDataCache& server_data_cache)
		:_wiki_data_cache(wiki_data_cache),
		_server_data_cache(server_data_cache)
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
			if(category_id >= _wiki_data_cache.category_has_article().size())
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


		Shared::StepTimer timer;

		timer.start_timing_step("build_event_network", "Build event network", &std::cout);
		const auto& subgraph = _server_data_cache.get_event_network(category_id);
		timer.stop_timing_step("build_event_network", &std::cout);

		// build list of articles in category
		timer.start_timing_step("build_event_list", "Build event list", &std::cout);
		const auto& event_list = _server_data_cache.get_event_list(category_id);
		Array events_array;
		for (auto& event : event_list) {
			Object el;
			el.set("title", event.Title);
			el.set("date", to_iso_string(event.Date));

			events_array.add(el);
		}
		timer.stop_timing_step("build_event_list", &std::cout);

		timer.start_timing_step("build_links_array", "Build links array", &std::cout);
		Array links_array;
		for (auto e : boost::make_iterator_range(boost::edges(subgraph))) {

			Array link;
			link.set(0, boost::source(e, subgraph));
			link.set(1, boost::target(e, subgraph));

			links_array.add(link);
		}
		timer.stop_timing_step("build_links_array", &std::cout);

		timer.start_timing_step("build_positions", "Build positions", &std::cout);
		const auto& positions = _server_data_cache.get_network_positions(category_id);
		Array positions_array;
		for (auto pos : positions) 
			positions_array.add(pos);
		timer.stop_timing_step("build_positions", &std::cout);

		timer.start_timing_step("build_main_path", "Build main path", &std::cout);
		const auto& main_path = _server_data_cache.get_global_main_path(category_id);
		Array mpa_array;
		for (auto edge : main_path) {
			Array link;
			link.set(0, edge.first);
			link.set(1, edge.second);

			mpa_array.add(link);
		}
		timer.stop_timing_step("build_main_path", &std::cout);

		Object root;
		root.set("events", events_array);
		root.set("links", links_array);
		root.set("positions", positions_array);
		root.set("main_path", mpa_array);

		root.stringify(response.send(), 4); 
	}

}
