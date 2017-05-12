#include <boost/functional/hash.hpp>
#include "../core/date.h"
#include "filters.h"
#include <boost/algorithm/string.hpp>

class RequestParameters {
	public:
		enum Method {
			LOCAL,
			GLOBAL,
			ALPHA
		};

		bool has_date_filter;
		int start_year;
		int end_year;
		Method method;
		double alpha;
		bool no_persons;
		std::string not_containing;



		std::size_t hash() const {
			std::size_t seed = 0;
			boost::hash_combine(seed, has_date_filter);
			boost::hash_combine(seed, start_year);
			boost::hash_combine(seed, end_year);
			boost::hash_combine(seed, method);
			boost::hash_combine(seed, alpha);
			boost::hash_combine(seed, no_persons);
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
