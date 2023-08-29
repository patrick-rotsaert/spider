#pragma once

#include "spider/aliases.h"
#include "spider/api.h"

#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/empty_body.hpp>

namespace spider {

SPIDER_EXPORT http::response<http::string_body>&& log_response(http::response<http::string_body>&& res);
SPIDER_EXPORT http::response<http::empty_body>&& log_response(http::response<http::empty_body>&& res);

} // namespace spider
