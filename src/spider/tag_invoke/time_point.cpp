#include "spider/tag_invoke/time_point.h"
#include "spider/conversions.h"

namespace std::chrono {

void tag_invoke(const boost::json::value_from_tag&, boost::json::value& out, const system_clock::time_point& in)
{
	out = spider::time_point_to_string(in);
}

system_clock::time_point tag_invoke(const boost::json::value_to_tag<system_clock::time_point>&, const boost::json::value& in)
{
	return spider::string_to_time_point(in.as_string());
}

} // namespace std::chrono
