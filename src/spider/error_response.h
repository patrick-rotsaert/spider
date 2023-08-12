//
// Copyright (C) 2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "spider/aliases.h"
#include "spider/api.h"

#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>

#include <optional>

namespace spider {

class SPIDER_EXPORT error_response_factory final
{
	using response = http::response<http::string_body>;
	using request  = http::request<http::string_body>;

public:
	static response create(const request& req, http::status status, const std::optional<beast::string_view>& html = std::nullopt);
};

template<http::status status>
class error_response
{
	using response = http::response<http::string_body>;
	using request  = http::request<http::string_body>;

public:
	static response create(const request& req)
	{
		return error_response_factory::create(req, status);
	}
};

using bad_request           = error_response<http::status::bad_request>;
using not_found             = error_response<http::status::not_found>;
using internal_server_error = error_response<http::status::internal_server_error>;

} // namespace spider
