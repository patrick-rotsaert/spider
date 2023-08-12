//
// Copyright (C) 2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

namespace boost {
namespace asio {
namespace ip {
class tcp;
}
} // namespace asio
namespace beast {
namespace http {
}
namespace websocket {
}
} // namespace beast
} // namespace boost

namespace spider {

namespace beast     = boost::beast;     // from <boost/beast.hpp>
namespace http      = beast::http;      // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net       = boost::asio;      // from <boost/asio.hpp>
using tcp           = net::ip::tcp;     // from <boost/asio/ip/tcp.hpp>

} // namespace spider
