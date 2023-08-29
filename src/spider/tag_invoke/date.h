#pragma once

#include "spider/api.h"

#include <boost/json/conversion.hpp>

#include <chrono>

namespace std::chrono {

void SPIDER_EXPORT           tag_invoke(const boost::json::value_from_tag&, boost::json::value& out, const year_month_day& in);
year_month_day SPIDER_EXPORT tag_invoke(const boost::json::value_to_tag<year_month_day>&, const boost::json::value& in);

} // namespace std::chrono
