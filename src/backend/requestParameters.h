#include <boost/functional/hash.hpp>
#include "../core/date.h"
#include "filters.h"

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



		std::size_t hash() const {
			std::size_t seed = 0;
			boost::hash_combine(seed, has_date_filter);
			boost::hash_combine(seed, start_year);
			boost::hash_combine(seed, end_year);
			boost::hash_combine(seed, method);
			boost::hash_combine(seed, alpha);

			return seed;
		};

		
		template<typename FType>
			using Filters = std::vector<std::function<FType>>;

		Filters<bool(const std::string&, const std::vector<Date>&)> article_filters() const {
			Filters<bool(const std::string&, const std::vector<Date>&)> rtn;

			rtn.push_back(articles_without_certain_dates({ "Birth", "Death", "Released", "Recorded", "First aired", "Last aired", "Term started", "Term ended" }));
			
			return rtn;
		}

		Filters<bool(const std::string&, const Date&)> event_filters() const {

			Filters<bool(const std::string&, const Date&)> rtn; 

			if(has_date_filter)
				rtn.push_back(events_in_date_range(create_date_range(start_year,1,1,end_year,1,1)));

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
