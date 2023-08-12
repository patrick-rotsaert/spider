//
// Copyright (C) 2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "spider/ilogger.h"
#include "spider/api.h"
#include "spider/config.h"

#include <fmt/format.h>
#include <boost/assert/source_location.hpp>

#include <memory>
#include <chrono>

namespace spider {

class SPIDER_EXPORT logging
{
public:
	static std::unique_ptr<ilogger> logger;

	// Not thread safe
	static void set_logger(std::unique_ptr<ilogger>&& logger);
};

} // namespace spider

// Macro for logging using a fmtlib format string
#define slog(lvl, ...)                                                                                                                     \
	do                                                                                                                                     \
	{                                                                                                                                      \
		auto& logger = ::spider::logging::logger;                                                                                          \
		if (logger)                                                                                                                        \
		{                                                                                                                                  \
			logger->log_message(                                                                                                           \
			    std::chrono::system_clock::now(), BOOST_CURRENT_LOCATION, ::spider::log_level::lvl, fmt::format(__VA_ARGS__));             \
		}                                                                                                                                  \
	} while (false)

// Macros for eliding logging code at compile time
#undef SPIDER_MIN_LOG
#define SPIDER_MIN_LOG(minlvl, lvl, ...)                                                                                                   \
	do                                                                                                                                     \
	{                                                                                                                                      \
		if constexpr (::spider::log_level::lvl >= ::spider::log_level::minlvl)                                                             \
		{                                                                                                                                  \
			slog(lvl, __VA_ARGS__);                                                                                                        \
		}                                                                                                                                  \
	} while (false)

#undef SPIDER_LOG
#define SPIDER_LOG(lvl, ...) SPIDER_MIN_LOG(SPIDER_LOGGING_LEVEL, lvl, __VA_ARGS__)
