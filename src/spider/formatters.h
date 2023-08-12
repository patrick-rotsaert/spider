//
// Copyright (C) 2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <boost/optional.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/date_time/gregorian/gregorian_io.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio/ip/basic_endpoint.hpp>
#include <boost/utility/string_view.hpp>
#include <boost/beast/core/string_type.hpp>
#include <boost/system/error_code.hpp>

#include <filesystem>
#include <iomanip>
#include <optional>

namespace fmt {

#if FMT_VERSION < 90000 // TODO: check which version introduced fmt::streamed

namespace detail {

template<typename T>
struct streamed_view
{
	const T& value;
};

} // namespace detail

// Formats an object of type T that has an overloaded ostream operator<<.
template<typename Char>
struct basic_ostream_formatter : formatter<basic_string_view<Char>, Char>
{
	void set_debug_format() = delete;

	template<typename T, typename OutputIt>
	auto format(const T& value, basic_format_context<OutputIt, Char>& ctx) const -> OutputIt
	{
		auto buffer = basic_memory_buffer<Char>();
		detail::format_value(buffer, value, ctx.locale());
		return formatter<basic_string_view<Char>, Char>::format({ buffer.data(), buffer.size() }, ctx);
	}
};

using ostream_formatter = basic_ostream_formatter<char>;

template<typename T, typename Char>
struct formatter<detail::streamed_view<T>, Char> : basic_ostream_formatter<Char>
{
	template<typename OutputIt>
	auto format(detail::streamed_view<T> view, basic_format_context<OutputIt, Char>& ctx) const -> OutputIt
	{
		return basic_ostream_formatter<Char>::format(view.value, ctx);
	}
};

template<typename T>
auto streamed(const T& value) -> detail::streamed_view<T>
{
	return { value };
}

#endif

// FIXME: move implementations to cpp

template<typename T>
struct formatter<std::optional<T>> : formatter<T>
{
	template<typename FormatContext>
	auto format(const std::optional<T>& opt, FormatContext& ctx) const
	{
		if (opt)
		{
			fmt::formatter<T>::format(opt.value(), ctx);
			return ctx.out();
		}
		return fmt::format_to(ctx.out(), "--");
	}
};

template<typename T>
struct formatter<boost::optional<T>> : formatter<T>
{
	template<typename FormatContext>
	auto format(const boost::optional<T>& opt, FormatContext& ctx) const
	{
		if (opt)
		{
			fmt::formatter<T>::format(opt.value(), ctx);
			return ctx.out();
		}
		return fmt::format_to(ctx.out(), "--");
	}
};

template<>
struct formatter<boost::uuids::uuid> : formatter<string_view>
{
	auto format(const boost::uuids::uuid& value, format_context& ctx) const
	{
		return formatter<string_view>::format(boost::uuids::to_string(value), ctx);
	}
};

template<>
struct formatter<boost::string_view> : formatter<string_view>
{
	auto format(boost::string_view value, format_context& ctx) const
	{
		return formatter<string_view>::format(string_view{ value.data(), value.length() }, ctx);
	}
};

template<>
struct formatter<boost::beast::string_view> : formatter<string_view>
{
	auto format(boost::beast::string_view value, format_context& ctx) const
	{
		return formatter<string_view>::format(string_view{ value.data(), value.length() }, ctx);
	}
};

template<>
struct formatter<boost::filesystem::path> : formatter<string_view>
{
	auto format(const boost::filesystem::path& value, format_context& ctx) const
	{
		return fmt::format_to(ctx.out(), "{}", fmt::streamed(value));
	}
};

template<>
struct formatter<std::filesystem::path> : formatter<string_view>
{
	auto format(const std::filesystem::path& value, format_context& ctx) const
	{
		return fmt::format_to(ctx.out(), "{}", fmt::streamed(std::quoted(value.string())));
	}
};

template<>
struct formatter<boost::posix_time::ptime> : formatter<string_view>
{
	auto format(const boost::posix_time::ptime& value, format_context& ctx) const
	{
		return fmt::format_to(ctx.out(), "{}", fmt::streamed(value));
	}
};

template<>
struct formatter<boost::posix_time::time_duration> : formatter<string_view>
{
	auto format(const boost::posix_time::time_duration& value, format_context& ctx) const
	{
		return fmt::format_to(ctx.out(), "{}", fmt::streamed(value));
	}
};

template<>
struct formatter<boost::gregorian::date> : formatter<string_view>
{
	auto format(const boost::gregorian::date& value, format_context& ctx) const
	{
		return fmt::format_to(ctx.out(), "{}", fmt::streamed(value));
	}
};

template<>
struct formatter<boost::system::error_code> : formatter<string_view>
{
	auto format(const boost::system::error_code& value, format_context& ctx) const
	{
		return formatter<string_view>::format(value.to_string(), ctx);
	}
};

template<typename InternetProtocol>
struct formatter<boost::asio::ip::basic_endpoint<InternetProtocol>> : formatter<string_view>
{
	auto format(const boost::asio::ip::basic_endpoint<InternetProtocol>& value, format_context& ctx) const
	{
		return fmt::format_to(ctx.out(), "{}", fmt::streamed(value));
	}
};

} // namespace fmt