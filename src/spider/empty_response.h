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

#include <boost/beast/http/message.hpp>
#include <boost/beast/http/empty_body.hpp>

namespace spider {

class SPIDER_EXPORT empty_response final
{
	using empty = http::response<http::empty_body>;

public:
	static empty create(const request& req, http::status status);
	static empty create(http::status status);
};

} // namespace spider
