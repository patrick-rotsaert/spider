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

#include <boost/url/url_view.hpp>
#include <boost/json.hpp>

#include <memory>
#include <regex>
#include <set>
#include <functional>

namespace spider {

template<typename T>
concept ConvertibleFromBoostJson = requires(const T& in)
{
	{
		boost::json::value_to<T>(boost::json::value{})
		} -> std::same_as<T>;
};

class SPIDER_EXPORT request_router : public irequest_handler
{
	class impl;
	std::unique_ptr<impl> pimpl_;
	friend impl; // allow impl to call private method route_request

	using verb              = http::verb;
	using request           = irequest_handler::request;
	using url_view          = boost::urls::url_view;
	using string_view       = beast::string_view;
	using svmatch           = std::match_results<string_view::const_iterator>;
	using message_generator = http::message_generator;
	using request_handler   = std::function<message_generator(request&& req, url_view&& url, string_view path, const svmatch& match)>;
	template<ConvertibleFromBoostJson T>
	using json_request_handler = std::function<
	    message_generator(request&& req, url_view&& url, string_view path, const svmatch& match, boost::json::result<T>&& data)>;

	message_generator handle_request(request&& req) override;
	message_generator route_request(request&& req, url_view&& url, string_view path);

public:
	request_router();
	~request_router() noexcept override;

	void add_route(std::set<verb>&& methods, std::regex&& pattern, request_handler&& handler);
	void add_route(std::set<verb>&& methods, std::regex&& pattern, const std::shared_ptr<request_router>& router);

	void add_route(verb method, std::regex&& pattern, request_handler&& handler);
	void add_route(verb method, std::regex&& pattern, const std::shared_ptr<request_router>& router);

	void add_route(std::regex&& pattern, request_handler&& handler);
	void add_route(std::regex&& pattern, const std::shared_ptr<request_router>& router);

	template<ConvertibleFromBoostJson T>
	void add_json_route(std::set<verb>&& methods, std::regex&& pattern, json_request_handler<T>&& handler)
	{
		this->add_route(
		    std::move(methods),
		    std::move(pattern),
		    [handler = std::move(handler)](request&& req, url_view&& url, string_view path, const svmatch& match) -> message_generator {
			    using result = boost::json::result<T>;

			    auto       ec = std::error_code{};
			    const auto jv = boost::json::parse(req.body(), ec);
			    if (ec)
			    {
				    return handler(std::move(req), std::move(url), path, match, result{ result::in_place_error, ec });
			    }
			    else
			    {
				    return handler(std::move(req), std::move(url), path, match, boost::json::try_value_to<T>(jv));
			    }
		    });
	}

	template<ConvertibleFromBoostJson T>
	void add_json_route(verb method, std::regex&& pattern, json_request_handler<T>&& handler)
	{
		this->add_json_route<T>(std::set<verb>{ method }, std::move(pattern), std::move(handler));
	}

	template<ConvertibleFromBoostJson T>
	void add_json_route(std::regex&& pattern, json_request_handler<T>&& handler)
	{
		this->add_json_route<T>(std::set<verb>{}, std::move(pattern), std::move(handler));
	}
};

} // namespace spider
