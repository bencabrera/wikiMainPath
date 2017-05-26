#include <boost/functional/hash.hpp>
#include "../core/date.h"
#include "filters.h"
#include <boost/algorithm/string.hpp>

class RequestParameters {
	public:
		enum Method {
			LOCAL,
			GLOBAL,
			ALPHA,
			ALPHA_LENGTH
		};

		bool has_date_filter;
		int start_year;
		int end_year;
		Method method;
		double alpha;
		std::size_t main_path_length;
		bool no_persons;
		bool not_recursive;
		std::string not_containing;

		RequestParameters()
			:has_date_filter(false),
			start_year(0),
			end_year(0),
			method(LOCAL),
			alpha(0.0),
			main_path_length(0),
			no_persons(false),
			not_recursive(false),
			not_containing("")
		{}


		std::size_t hash() const {
			std::size_t seed = 0;
			boost::hash_combine(seed, has_date_filter);
			boost::hash_combine(seed, start_year);
			boost::hash_combine(seed, end_year);
			boost::hash_combine(seed, method);
			boost::hash_combine(seed, alpha);
			boost::hash_combine(seed, main_path_length);
			boost::hash_combine(seed, no_persons);
			boost::hash_combine(seed, not_recursive);
			boost::hash_combine(seed, not_containing);

			return seed;
		};

		
		template<typename FType>
			using Filters = std::vector<std::function<FType>>;

		Filters<bool(const std::string&, const std::vector<Date>&)> article_filters() const {
			Filters<bool(const std::string&, const std::vector<Date>&)> rtn;

			if(no_persons)
				rtn.push_back(articles_without_certain_dates({ "Birth", "Death", "Released", "Recorded", "First aired", "Last aired", "Term started", "Term ended" }));
			
			return rtn;
		}

		Filters<bool(const std::string&, const Date&)> event_filters() const {

			Filters<bool(const std::string&, const Date&)> rtn; 

			if(has_date_filter)
				rtn.push_back(events_in_date_range(create_date_range(start_year,1,1,end_year,1,1)));

			if(not_containing != "")
			{
				std::vector<std::string> contain_str;	
				boost::split(contain_str, not_containing, boost::is_any_of(","));
				for (auto& str : contain_str) {
					boost::to_lower(str);	
				}
				rtn.push_back(events_not_containing(contain_str));
			}

			return rtn;
		}
};

namespace std {
	template<> struct hash<RequestParameters> {

		std::size_t operator()(const RequestParameters& params) const
		{
			return params.hash();
		}

	};
}
