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

#include <boost/asio/ip/tcp.hpp>

#include <memory>

namespace spider {

// Handles an HTTP server connection
class SPIDER_EXPORT http_session : public std::enable_shared_from_this<http_session>
{
	static constexpr std::size_t queue_limit = 8; // max number of queued responses

public:
	static void run(tcp::socket&& socket, const std::shared_ptr<irequest_handler>& request_handler);
};

} // namespace spider
