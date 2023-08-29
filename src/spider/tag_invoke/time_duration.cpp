#include "spider/tag_invoke/time_duration.h"
#include "spider/conversions.h"

namespace boost::posix_time {

void tag_invoke(const boost::json::value_from_tag&, boost::json::value& out, const time_duration& in)
{
	out = spider::boost_time_duration_to_string(in);
}

time_duration tag_invoke(const boost::json::value_to_tag<time_duration>&, const boost::json::value& in)
{
	return spider::string_to_boost_time_duration(in.as_string());
}

} // namespace boost::posix_time
