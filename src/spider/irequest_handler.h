//
// Copyright (C) 2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "spider/aliases.h"
#include "spider/message.h"
#include "spider/api.h"

namespace spider {

class SPIDER_EXPORT irequest_handler
{
public:
	irequest_handler();
	virtual ~irequest_handler() noexcept;

	irequest_handler(const irequest_handler&)            = delete;
	irequest_handler& operator=(const irequest_handler&) = delete;
	irequest_handler(irequest_handler&&)                 = default;
	irequest_handler& operator=(irequest_handler&&)      = default;

	virtual message_generator handle_request(request&& req) = 0;
};

} // namespace spider
