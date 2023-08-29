#include "spider/tag_invoke/date.h"
#include "spider/conversions.h"

namespace std::chrono {

void tag_invoke(const boost::json::value_from_tag&, boost::json::value& out, const year_month_day& in)
{
	out = spider::date_to_string(in);
}

year_month_day tag_invoke(const boost::json::value_to_tag<year_month_day>&, const boost::json::value& in)
{
	return spider::string_to_date(in.as_string());
}

} // namespace std::chrono
