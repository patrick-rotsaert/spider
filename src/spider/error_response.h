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

#include <optional>

namespace spider {

class SPIDER_EXPORT error_response_factory final
{
public:
	static response create(const request& req, http::status status, const std::optional<string_view>& html = std::nullopt);
};

template<http::status status>
class error_response
{
public:
	static response create(const request& req)
	{
		return error_response_factory::create(req, status);
	}
};

using bad_request           = error_response<status::bad_request>;
using not_found             = error_response<status::not_found>;
using internal_server_error = error_response<status::internal_server_error>;
using not_implemented       = error_response<status::not_implemented>;

} // namespace spider
