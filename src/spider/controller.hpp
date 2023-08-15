//
// Copyright (C) 2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

// Code in this file is inspired by https://github.com/mhekkel/libzeep

// Copyright Maarten L. Hekkelman, Radboud University 2008-2013.
//        Copyright Maarten L. Hekkelman, 2014-2022
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "spider/request_router.h"
#include "spider/error_response.h"
#include "spider/response_wrapper.h"
#include "spider/conversions.h"
#include "spider/demangled_type_name.h"
#include "spider/logging.h"
#include "spider/formatters.h"

#include <boost/beast/core/string.hpp>

#include <fmt/format.h>

#include <type_traits>
#include <tuple>
#include <string>
#include <variant>
#include <array>
#include <iomanip>

namespace spider {

struct path_parameter
{
	std::string name;
};

struct query_parameter
{
	std::string name;
};

struct header_parameter
{
	std::string name;
};

struct json_body
{
	std::string name;
};

struct request_parameter
{
};

class controller
{
	using svmatch = boost::match_results<string_view::const_iterator>;

	using parameter_descriptor = std::variant<path_parameter, query_parameter, header_parameter, json_body, request_parameter>;

	struct parameter_sources
	{
		const request&     req;
		const url_view&    url;
		const string_view& path;
		const svmatch&     match;
	};

	class argument_error : public std::invalid_argument
	{
	public:
		explicit argument_error(const std::string& m)
		    : std::invalid_argument{ m }
		{
		}
	};

	template<typename Dispatcher, typename...>
	struct handler
	{
	};

	template<typename Dispatcher, typename Result, typename... Args>
	struct handler<Result (Dispatcher::*)(Args...)>
	{
		using Method     = Result (Dispatcher::*)(Args...);
		using ArgsTuple  = std::tuple<typename std::remove_const_t<typename std::remove_reference_t<Args>>...>;
		using ResultType = typename std::remove_const_t<typename std::remove_reference_t<Result>>;
		using Callback   = std::function<ResultType(Args...)>;

		static constexpr size_t N = sizeof...(Args);

		Callback                            callback_;
		std::array<parameter_descriptor, N> descriptors_;

		template<typename... Descriptors>
		handler(controller* owner, Method method, Descriptors... descriptors)
		    : callback_{}
		    , descriptors_{}
		{
			static_assert(sizeof...(descriptors) == sizeof...(Args),
			              "The number of descriptors must be equal to the number of arguments of the callback function");

			auto dispatcher = dynamic_cast<Dispatcher*>(owner);
			if (dispatcher == nullptr)
			{
				throw std::runtime_error("Callback must be provided by a class derived from dispatcher");
			}

			this->callback_ = [dispatcher, method](Args... args) { return (dispatcher->*method)(args...); };

			if constexpr (sizeof...(descriptors) > 0)
			{
				auto i = size_t{ 0 };
				for (auto name : { parameter_descriptor{ descriptors }... })
				{
					this->descriptors_[i++] = name;
				}
			}
		}

		ResultType call(const parameter_sources& sources)
		{
			try
			{
				return invoke(collect_arguments(sources, std::make_index_sequence<N>()));
			}
			catch (const argument_error& e)
			{
				// argument_error is private, so we're sure the exception comes from arguments collection
				SPIDER_LOG(err, "Could not collect arguments: {}", e.what());
				return bad_request::create();
			}
		}

		template<typename ArgsTuple>
		ResultType invoke(ArgsTuple&& args)
		{
			return std::apply(this->callback_, std::forward<ArgsTuple>(args));
		}

		template<std::size_t... I>
		ArgsTuple collect_arguments(const parameter_sources& sources, std::index_sequence<I...>)
		{
			return std::make_tuple(collect_argument<typename std::tuple_element_t<I, ArgsTuple>>(
			    sources, this->descriptors_[I], static_cast<typename std::tuple_element_t<I, ArgsTuple>*>(0))...);
		}

		template<typename T>
		T collect_argument(const parameter_sources& sources, const parameter_descriptor& descriptor, const T* const tag)
		{
			return std::visit(
			    [&](auto&& p) -> T {
				    using P = std::decay_t<decltype(p)>;
				    if constexpr (std::is_same_v<P, path_parameter> && !ConvertibleFromBoostJson<T> && !std::is_pointer_v<T>)
				    {
					    const auto& m = sources.match[p.name];
					    if (m.matched)
					    {
						    const auto sv = std::string_view{ m.first, m.second };
						    try
						    {
							    return get_argument(sv, tag);
						    }
						    catch (const std::exception& e)
						    {
							    throw argument_error{ fmt::format("'{}': could not convert {} to {}: {}",
								                                  p.name,
								                                  fmt::streamed(std::quoted(sv)),
								                                  demangled_type_name<T>(),
								                                  e.what()) };
						    }
					    }
				    }
				    else if constexpr (std::is_same_v<P, query_parameter> && !ConvertibleFromBoostJson<T> && !std::is_pointer_v<T>)
				    {
					    const auto it = sources.url.params().find(p.name);
					    if (it != sources.url.params().end())
					    {
						    const auto& sv = (*it).value;
						    try
						    {
							    return get_argument(sv, tag);
						    }
						    catch (const std::exception& e)
						    {
							    throw argument_error{ fmt::format("'{}': could not convert {} to {}: {}",
								                                  p.name,
								                                  fmt::streamed(std::quoted(sv)),
								                                  demangled_type_name<T>(),
								                                  e.what()) };
						    }
					    }
				    }
				    else if constexpr (std::is_same_v<P, header_parameter> && !ConvertibleFromBoostJson<T> && !std::is_pointer_v<T>)
				    {
					    const auto it = sources.req.find(p.name);
					    if (it != sources.req.end())
					    {
						    // TODO: check if the value can be obtained from the iterator, to avoid the second lookup.
						    //auto sv = string_view{ *it };
						    auto sv = sources.req[p.name];
						    try
						    {
							    return get_argument(sv, tag);
						    }
						    catch (const std::exception& e)
						    {
							    throw argument_error{ fmt::format("'{}': could not convert {} to {}: {}",
								                                  p.name,
								                                  fmt::streamed(std::quoted(std::string{ sv })),
								                                  demangled_type_name<T>(),
								                                  e.what()) };
						    }
					    }
				    }
				    else if constexpr (std::is_same_v<P, json_body> && ConvertibleFromBoostJson<T> && !std::is_pointer_v<T>)
				    {
					    try
					    {
						    return boost::json::value_to<T>(boost::json::parse(sources.req.body()));
					    }
					    catch (const std::exception& e)
					    {
						    throw argument_error{ fmt::format(
							    "'{}': could not convert request body to {}: {}", p.name, demangled_type_name<T>(), e.what()) };
					    }
				    }
				    else if constexpr (std::is_same_v<P, request_parameter> && std::is_pointer_v<T>)
				    {
					    return &sources.req;
				    }
				    return T{};
			    },
			    descriptor);
		}

		std::string get_argument(std::string_view in, const std::string* const)
		{
			return std::string{ in };
		}

		std::string_view get_argument(std::string_view in, const std::string_view* const)
		{
			return in;
		}

		string_view get_argument(std::string_view in, const string_view* const)
		{
			return in;
		}

		bool get_argument(std::string_view in, const bool* const)
		{
			if (beast::iequals(in, "true") || beast::iequals(in, "yes") || beast::iequals(in, "on"))
			{
				return true;
			}
			else if (beast::iequals(in, "false") || beast::iequals(in, "no") || beast::iequals(in, "off"))
			{
				return false;
			}
			else
			{
				return string_to_number<int>(in) != 0;
			}
		}

		boost::gregorian::date get_argument(std::string_view in, const boost::gregorian::date* const)
		{
			return string_to_boost_date(in);
		}

		boost::posix_time::time_duration get_argument(std::string_view in, const boost::posix_time::time_duration* const)
		{
			return string_to_boost_time_duration(in);
		}

		boost::posix_time::ptime get_argument(std::string_view in, const boost::posix_time::ptime* const)
		{
			return string_to_boost_ptime(in);
		}

		date get_argument(std::string_view in, const date* const)
		{
			return string_to_date(in);
		}

		time_of_day get_argument(std::string_view in, const time_of_day* const)
		{
			return string_to_time_of_day(in);
		}

		time_point get_argument(std::string_view in, const time_point* const)
		{
			return string_to_time_point(in);
		}

		template<typename T>
		std::enable_if_t<std::is_scalar_v<T>, T> get_argument(std::string_view in, const T* const)
		{
			return string_to_number<T>(in);
		}

		template<typename T>
		std::optional<T> get_argument(std::string_view in, const std::optional<T>* const)
		{
			return get_argument(in, static_cast<T*>(0));
		}

		template<typename T>
		boost::optional<T> get_argument(std::string_view in, const boost::optional<T>* const)
		{
			return get_argument(in, static_cast<T*>(0));
		}
	};

	std::shared_ptr<request_router> router_;

public:
	virtual ~controller()
	{
	}

	explicit controller(std::shared_ptr<request_router> router)
	    : router_{ std::move(router) }
	    , exception_handler_{}
	{
	}

	const std::shared_ptr<request_router>& router() const
	{
		return this->router_;
	}

	class exception_handler_base
	{
	public:
		virtual ~exception_handler_base()
		{
		}

		virtual response handle(const std::exception& e, const request& req) = 0;
	};

	void exception_handler(const std::shared_ptr<exception_handler_base>& value)
	{
		this->exception_handler_ = value;
	}

private:
	std::shared_ptr<exception_handler_base> exception_handler_;

protected:
	template<class Callback, typename... ArgDescriptors>
	void register_action(std::set<verb>&& methods, boost::regex&& pattern, Callback callback, ArgDescriptors... descriptors)
	{
		// Note: the closure passed to add_route must be copyable.
		// This is why the handler is created as a shared_ptr, rather than a unique_ptr.
		// I would have preferred to use a unique_ptr, but then the handler would need to be
		// move-captured, thus making the lambda non-copyable and std::function would fail to create.
		auto h = std::make_shared<handler<Callback>>(this, callback, descriptors...);
		this->router_->add_route(
		    std::move(methods),
		    std::move(pattern),
		    [handler = h, this](request&& req, url_view&& url, string_view path, const svmatch& match) -> response_wrapper {
			    try
			    {
				    auto res = handler->call(parameter_sources{ req, url, path, match });
				    res.version(req.version());
				    res.keep_alive(req.keep_alive());
				    return res;
			    }
			    catch (const std::exception& e)
			    {
				    SPIDER_LOG(err, "{}", e.what());
				    if (this->exception_handler_)
				    {
					    return this->exception_handler_->handle(e, req);
				    }
				    else
				    {
					    return internal_server_error::create(req);
				    }
			    }
		    });
	}
};

} // namespace spider
