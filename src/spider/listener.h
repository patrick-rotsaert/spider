//
// Copyright (C) 2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "spider/irequest_handler.h"
#include "spider/aliases.h"
#include "spider/api.h"

#include <string_view>
#include <cstdint>
#include <memory>

namespace spider {

// Accepts incoming connections and launches the sessions
class SPIDER_EXPORT listener
{
public:
	// Start accepting incoming connections
	static void run(net::io_context&                         ioc,
	                const std::string_view&                  address,
	                std::uint16_t                            port,
	                const std::shared_ptr<irequest_handler>& request_handler,
	                beast::error_code&                       ec);
};

} // namespace spider
