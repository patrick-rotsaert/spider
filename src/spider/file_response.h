//
// Copyright (C) 2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "spider/ifile_event_listener.h"
#include "spider/aliases.h"
#include "spider/api.h"

#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/message_generator.hpp>
#include <boost/filesystem/path.hpp>

#include <memory>

namespace spider {

class SPIDER_EXPORT file_response final
{
public:
	using request  = http::request<http::string_body>;
	using response = http::message_generator;

	static response create(const request& req, const boost::filesystem::path& doc_root, beast::string_view path);
	static response create(const request&                          req,
	                       const boost::filesystem::path&          doc_root,
	                       beast::string_view                      path,
	                       std::unique_ptr<ifile_event_listener>&& event_listener);
};

} // namespace spider
