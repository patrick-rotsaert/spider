//
// Copyright (C) 2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "spider/ilogger.h"

#include <mutex>

namespace spider {

class example_logger : public ilogger
{
	std::mutex mutex_;

public:
	example_logger();
	~example_logger() override;

	void log_message(const std::chrono::system_clock::time_point& time,
	                 const boost::source_location&                location,
	                 log_level                                    level,
	                 const std::string_view&                      message) override;
};

} // namespace spider
