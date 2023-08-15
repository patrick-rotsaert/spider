//
// Copyright (C) 2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "spider/ifile_event_listener.h"
#include "spider/aliases.h"
#include "spider/message.h"
#include "spider/api.h"

#include <memory>

namespace spider {

class SPIDER_EXPORT file_response final
{
public:
	static message_generator create(const request& req, const fs::path& doc_root, string_view path);
	static message_generator
	create(const request& req, const fs::path& doc_root, string_view path, std::unique_ptr<ifile_event_listener>&& event_listener);
};

} // namespace spider
