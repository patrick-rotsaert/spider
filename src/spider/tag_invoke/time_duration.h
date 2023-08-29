#pragma once

#include "spider/api.h"

#include <boost/json/conversion.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>

namespace boost::posix_time {

void SPIDER_EXPORT          tag_invoke(const boost::json::value_from_tag&, boost::json::value& out, const time_duration& in);
time_duration SPIDER_EXPORT tag_invoke(const boost::json::value_to_tag<time_duration>&, const boost::json::value& in);

} // namespace boost::posix_time
