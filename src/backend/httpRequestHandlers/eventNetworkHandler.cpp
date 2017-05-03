#include "eventNetworkHandler.h"

#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/exception/diagnostic_information.hpp> 

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
				std::cerr << "Aborting request" << std::endl;
				return;
			}
		}
		catch(std::exception& e)
		{
			Object root;
			root.set("error", e.what());
			root.stringify(response.send(), 4); 
			std::cerr << "Aborting request" << std::endl;
			return;
		}

		std::cout << std::endl << "===============================================================" << std::endl;

		Shared::StepTimer timer;

		Array events_array, links_array, positions_array, mpa_array, timespan_array;

		try {
			timer.start_timing_step("build_event_network", "Build event network", &std::cout);
			const auto& subgraph = _server_data_cache.get_event_network(category_id);
			timer.stop_timing_step("build_event_network", &std::cout);

			// build list of articles in category
			timer.start_timing_step("build_event_list", "Build event list", &std::cout);
			const auto& event_list = _server_data_cache.get_event_list(category_id);
			for (auto& event : event_list) {
				Object el;
				el.set("title", event.Title);
				el.set("date", to_iso_string(event.Date));

				events_array.add(el);
			}
			timer.stop_timing_step("build_event_list", &std::cout);

			timer.start_timing_step("build_links_array", "Build links array", &std::cout);
			for (auto e : boost::make_iterator_range(boost::edges(subgraph))) 
			{
				Array link;
				link.set(0, boost::source(e, subgraph));
				link.set(1, boost::target(e, subgraph));

				links_array.add(link);
			}
			timer.stop_timing_step("build_links_array", &std::cout);

			timer.start_timing_step("build_positions", "Build positions", &std::cout);
			const auto& positions = _server_data_cache.get_network_positions(category_id);
			for (auto pos : positions) 
				positions_array.add(pos);
			timer.stop_timing_step("build_positions", &std::cout);

			timer.start_timing_step("build_main_path", "Build main path", &std::cout);
			const auto& main_path = _server_data_cache.get_global_main_path(category_id);
			for (auto edge : main_path) {
				Array link;
				link.set(0, edge.first);
				link.set(1, edge.second);

				mpa_array.add(link);
			}
			timer.stop_timing_step("build_main_path", &std::cout);

			auto min_date = std::min_element(event_list.begin(), event_list.end(), [](const ServerDataCache::Event& e1, const ServerDataCache::Event& e2) {
					return e1.Date < e2.Date;
					})->Date;
			auto max_date = std::max_element(event_list.begin(), event_list.end(), [](const ServerDataCache::Event& e1, const ServerDataCache::Event& e2) {
					return e1.Date < e2.Date;
					})->Date;
			timespan_array.set(0, to_iso_string(min_date));
			timespan_array.set(1, to_iso_string(max_date));
		}
		catch(boost::exception& e)
		{
			response.setStatus("400"); // = Bad Request
			Object root;
			root.set("error", boost::diagnostic_information(e, true));
			root.stringify(response.send(), 4); 
			std::cerr << "Aborting request" << std::endl;
			return;
		}
		catch(std::exception& e)
		{
			response.setStatus("400"); // = Bad Request
			Object root;
			root.set("error", e.what());
			root.stringify(response.send(), 4); 
			std::cerr << "Aborting request" << std::endl;
			return;
		}

		timer.start_timing_step("build_return_json", "Building JSON to return", &std::cout);

		Object root;
		root.set("events", events_array);
		root.set("links", links_array);
		root.set("positions", positions_array);
		root.set("main_path", mpa_array);
		root.set("category_title", _wiki_data_cache.category_titles()[category_id]);
		root.set("timespan", timespan_array);

		timer.stop_timing_step("build_return_json", &std::cout);

		std::cout << std::endl << "Timings: " << std::endl;
		timer.print_timings(std::cout);

		std::cout.flush();

		root.stringify(response.send(), 4); 
	}

}
