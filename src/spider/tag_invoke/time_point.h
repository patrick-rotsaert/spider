#pragma once

#include "spider/api.h"

#include <boost/json/conversion.hpp>

#include <chrono>

namespace std::chrono {

void SPIDER_EXPORT tag_invoke(const boost::json::value_from_tag&, boost::json::value& out, const system_clock::time_point& in);
system_clock::time_point SPIDER_EXPORT tag_invoke(const boost::json::value_to_tag<system_clock::time_point>&, const boost::json::value& in);

} // namespace std::chrono
