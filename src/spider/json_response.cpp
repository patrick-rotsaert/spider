//
// Copyright (C) 2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "spider/json_response.h"

#include <boost/beast/version.hpp>

namespace spider {

json_response::response spider::json_response::create_impl(const request& req, http::status status, std::string&& json)
{
	auto res = response{ status, req.version() };
	res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
	res.set(boost::beast::http::field::content_type, "application/json");
	res.keep_alive(req.keep_alive());
	res.body() = std::move(json);
	res.prepare_payload();
	return res;
}

} // namespace spider
