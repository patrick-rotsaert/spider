#include "spider/log_response.h"
#include "spider/logging.h"
#include "spider/formatters.h"

#include <boost/beast/http.hpp>

namespace spider {

namespace {

template<class Body>
inline auto&& log_response_impl(http::response<Body>&& res)
{
	SPIDER_LOG(trace, "response:\n{}", fmt::streamed(res));
	return std::move(res);
}

} // namespace

http::response<http::string_body>&& log_response(http::response<http::string_body>&& res)
{
	return log_response_impl(std::move(res));
}

http::response<http::empty_body>&& log_response(http::response<http::empty_body>&& res)
{
	return log_response_impl(std::move(res));
}

} // namespace spider
