//
// Copyright (C) 2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "spider/logging.h"
#include "spider/config.h"
#ifdef SPIDER_USE_SPDLOG
#include "spider/spdlog_logger.h"
#endif

namespace spider {

#ifdef SPIDER_USE_SPDLOG
std::unique_ptr<ilogger> logging::logger = std::make_unique<spdlog_logger>();
#else
std::unique_ptr<ilogger> logging::logger{};
#endif

void logging::set_logger(std::unique_ptr<ilogger>&& logger)
{
	logging::logger = std::move(logger);
}

} // namespace spider
