//
// Copyright (C) 2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "spider/aliases.h"
#include "spider/tracked_file.h"

#include <boost/beast/http/empty_body.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/file_body.hpp>
#include <boost/beast/http/message_generator.hpp>

#include <variant>

namespace spider {

class response_wrapper
{
	std::variant<http::response<http::basic_file_body<tracked_file>>,
	             http::response<http::file_body>,
	             http::response<http::string_body>,
	             http::response<http::empty_body>>
	    value_;

public:
	template<class R>
	response_wrapper(R&& r)
	    : value_{ std::move(r) }
	{
	}

	operator message_generator()
	{
		return std::visit([](auto&& arg) -> message_generator { return std::move(arg); }, this->value_);
	}

	void keep_alive(bool value)
	{
		std::visit([&](auto&& arg) { arg.keep_alive(value); }, this->value_);
	}

	void version(unsigned value)
	{
		std::visit([&](auto&& arg) { arg.version(value); }, this->value_);
	}
};

} // namespace spider
