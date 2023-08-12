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
#include <boost/json.hpp>

#include <concepts>

namespace spider {

template<typename T>
concept ConvertibleToBoostJson = requires(const T& in)
{
	{
		boost::json::value_from(in)
		} -> std::same_as<boost::json::value>;
};

class SPIDER_EXPORT json_response final
{
	using response = http::response<http::string_body>;
	using request  = http::request<http::string_body>;

	static response create_impl(const request& req, http::status status, std::string&& json);

public:
	template<ConvertibleToBoostJson T>
	static response create(const request& req, http::status status, const T& data)
	{
		return create_impl(req, status, boost::json::serialize(boost::json::value_from(data)));
	}

	template<ConvertibleToBoostJson T>
	static response create(const request& req, http::status status, T&& data)
	{
		return create_impl(req, status, boost::json::serialize(boost::json::value_from(std::move(data))));
	}
};

} // namespace spider
