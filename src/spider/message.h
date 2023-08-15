#pragma once

#include "aliases.h"

#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>

namespace spider {

using request  = http::request<http::string_body>;
using response = http::response<http::string_body>;

} // namespace spider
