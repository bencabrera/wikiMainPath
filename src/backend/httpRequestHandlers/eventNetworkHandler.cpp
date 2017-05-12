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

	EventNetworkHandler::EventNetworkHandler(const WikiDataCache& wiki_data_cache)
		:_wiki_data_cache(wiki_data_cache)
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
		std::map<std::string, std::string> parameter_map;
		
		for(auto& p : parameters)
			parameter_map.insert(p);

		if(parameters.size() == 0 || parameter_map.count("category-id") == 0)
		{
			response.setStatus("400"); // = Bad Request

			Object root;
			root.set("error", "GET-Parameter category-id not specified or too few parameters.");

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

		timer.start_timing_step("build_cache", "Build server data cache", &std::cout);
		ServerDataCache _server_data_cache(_wiki_data_cache);
		timer.stop_timing_step("build_cache", &std::cout);

		RequestParameters request_parameters{ false, 0, 0, RequestParameters::LOCAL, 0.0, false, "" };

		if(parameter_map.count("from-year") > 0 && parameter_map.count("to-year"))
		{
			int from_year = std::stoi(parameter_map.at("from-year"));
			int to_year = std::stoi(parameter_map.at("to-year"));

			request_parameters.has_date_filter = true;
			request_parameters.start_year = from_year;
			request_parameters.end_year = to_year;
		}

		request_parameters.method = RequestParameters::LOCAL;
		if(parameter_map.count("method") > 0)
		{
			auto method_str = parameter_map.at("method");
			if(method_str == "local")
				request_parameters.method = RequestParameters::LOCAL;
			if(method_str == "global")
				request_parameters.method = RequestParameters::GLOBAL;
			if(method_str == "alpha" && parameter_map.count("alpha") > 0)
			{
				request_parameters.method = RequestParameters::ALPHA;
				request_parameters.alpha = std::stod(parameter_map.at("alpha"));
			}
		}

		if(parameter_map.count("no_persons") > 0)
		{
			request_parameters.no_persons = true;
		}

		if(parameter_map.count("not_containing") > 0)
		{
			request_parameters.not_containing = parameter_map.at("not_containing");
		}

		Array events_array, links_array, positions_array, mpa_array, timespan_array;

		try {
			timer.start_timing_step("build_event_network", "Build event network", &std::cout);
			const auto& subgraph = _server_data_cache.get_event_network(category_id, request_parameters);
			timer.stop_timing_step("build_event_network", &std::cout);

			// build list of articles in category
			timer.start_timing_step("build_event_list", "Build event list", &std::cout);
			const auto& event_list = _server_data_cache.get_event_list(category_id, request_parameters);
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
			const auto& positions = _server_data_cache.get_network_positions(category_id, request_parameters);
			for (auto pos : positions) 
				positions_array.add(pos);
			timer.stop_timing_step("build_positions", &std::cout);

			timer.start_timing_step("build_main_path", "Build main path", &std::cout);
			const auto& main_path = _server_data_cache.get_global_main_path(category_id, request_parameters);
			for (auto edge : main_path) {
				Array link;
				link.set(0, edge.first);
				link.set(1, edge.second);

				mpa_array.add(link);
			}
			timer.stop_timing_step("build_main_path", &std::cout);

			auto min_date = std::min_element(event_list.begin(), event_list.end(), [](const ServerDataCache::Event& e1, const ServerDataCache::Event& e2) {
					return e1.Date < e2.Date;
					});
			auto max_date = std::max_element(event_list.begin(), event_list.end(), [](const ServerDataCache::Event& e1, const ServerDataCache::Event& e2) {
					return e1.Date < e2.Date;
					});

			if(min_date != event_list.end())
				timespan_array.set(0, to_iso_string(min_date->Date));
			else
				timespan_array.set(0, 0);

			if(max_date != event_list.end())
				timespan_array.set(1, to_iso_string(max_date->Date));
			else
				timespan_array.set(1, 0);
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
