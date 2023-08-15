#include "spider/controller.hpp"
#include "spider/listener.h"
#include "spider/error_response.h"
#include "spider/file_response.h"
#include "spider/json_response.h"
#include "spider/noop_file_event_listener.h"
#include "spider/logging.h"
#include "spider/formatters.h"
#include "spider/config.h"
#include "spider/aliases.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/describe.hpp>
#include <boost/throw_exception.hpp>

#ifdef SPIDER_USE_SPDLOG
#include <spdlog/spdlog.h>
#endif

#include <fmt/format.h>

#include <thread>

using namespace spider;
using namespace fmt::literals;

// Note: structs must be described at namespace scope.
struct customer
{
	std::uint64_t id;
	std::string   name;

	customer() = default;

	customer(std::uint64_t id, std::string name)
	    : id{ id }
	    , name{ name }
	{
	}

	// non-copyable
	customer(const customer&)            = delete;
	customer& operator=(const customer&) = delete;

	// moveable
	customer(customer&& other)      = default;
	customer& operator=(customer&&) = default;
};

BOOST_DESCRIBE_STRUCT(customer, (), (id, name))

/// Error handling
struct error
{
	std::string        message;
	std::optional<int> error_code;
};

BOOST_DESCRIBE_STRUCT(error, (), (message, error_code))

class api_exception_handler : public controller::exception_handler_base
{
	response handle(const std::exception& e, const request& req) override
	{
		auto err = error{ e.what() };
		// TODO: get error code from exception
		return json_response::create(req, status::internal_server_error, err);
	}
};

class api_controller : public controller
{
	auto get_customer(std::uint64_t id, const std::optional<std::string>& serial, const boost::optional<std::string>& api_key)
	{
		slog(debug, "get customer id={}, serial={}, api_key={}", id, serial, api_key);
		return json_response::create(status::ok, customer{ id, "The Customer Inc" });
	}

	auto post_customer(const customer& c)
	{
		slog(debug, "api post customer {}", c.id);
		return json_response::create(status::ok, c);
	}

	response fail(const request* req)
	{
		slog(debug, "fail, method is {}", req->method_string());
		throw std::runtime_error("The error message");
	}

public:
	explicit api_controller(const std::shared_ptr<request_router>& router = std::make_shared<request_router>())
	    : controller{ router }
	{
		this->register_action(
		    { verb::get },                                  // HTTP methods
		    boost::regex{ R"~(^customer/(?<id>\d+)/?$)~" }, // Path regex
		    &api_controller::get_customer,                  // Callback
		    // Descriptors for the callback arguments.
		    // There must be a descriptor for each argument and they must be passed in the same order as the callback arguments.
		    path_parameter{ "id" },         // Path parameters refer to a named sub-expression in the regex, i.e. `(?<id>\d+)`
		                                    //                                                                        ~~
		    query_parameter{ "serial" },    // Query parameters refer to the key name, e.g. http://localhost/api/customer/123?serial=123abc
		                                    //                                                                                ~~~~~~
		    header_parameter{ "x-api-key" } // Header parameters refer to the header name, e.g. `X-Api-Key: abc12345`
		                                    //                                                   ~~~~~~~~~
		);

		this->register_action(
		    { verb::post },                      // HTTP methods
		    boost::regex{ R"~(^customer/?$)~" }, // Path regex
		    &api_controller::post_customer,      // Callback
		    // Descriptors for the callback arguments.
		    // There must be a descriptor for each argument and they must be passed in the same order as the callback arguments.
		    json_body{ "customer" } // Json parameters are deserialized from the request payload.
		                            // The name ("customer") is used only in logging in case deserialization fails.
		);

		this->register_action({},                              // HTTP method(s). An empty set means all methods.
		                      boost::regex{ R"~(^fail/?$)~" }, // Path regex
		                      &api_controller::fail,           // Callback
		                      request_parameter{}              //
		);

		this->exception_handler(std::make_shared<api_exception_handler>());
	}
};

class file_controller : public controller
{
	boost::filesystem::path doc_root_;

	class file_event_listener final : public noop_file_event_listener
	{
		void on_close(const error_code& ec, std::uint64_t size, std::uint64_t pos) override
		{
			slog(info, "on close, size={}, pos={}", size, pos);
		}
	};

	auto get_file(const std::string& path, std::optional<bool> track)
	{
		slog(debug, "get file path={}, track={}", path, track);
		if (track.value_or(true))
		{
			return file_response::create(this->doc_root_, path, std::make_unique<file_event_listener>());
		}
		else
		{
			return file_response::create(this->doc_root_, path);
		}
	}

public:
	explicit file_controller(const boost::filesystem::path&         doc_root,
	                         const std::shared_ptr<request_router>& router = std::make_shared<request_router>())
	    : controller{ router }
	    , doc_root_{ doc_root }
	{
		this->register_action({ verb::get },
		                      boost::regex{ R"~(^(?<path>.+)/?$)~" },
		                      &file_controller::get_file,
		                      path_parameter{ "path" },
		                      query_parameter{ "track" });
	}
};

int main(int argc, char* argv[])
{
	// Check command line arguments.
	if (argc != 5)
	{
		fmt::print(stderr,
		           "Usage: {prog} <address> <port> <doc_root> <threads>\n"
		           "Example:\n"
		           "    {prog} 0.0.0.0 8080 . 1\n",
		           "prog"_a = argv[0]);
		return EXIT_FAILURE;
	}
	auto const address  = argv[1];
	auto const port     = static_cast<std::uint16_t>(std::atoi(argv[2]));
	auto const doc_root = boost::filesystem::path{ argv[3] };
	auto const threads  = std::max<int>(1, std::atoi(argv[4]));

#ifdef SPIDER_USE_SPDLOG
	spdlog::set_level(spdlog::level::trace);
	spdlog::set_pattern("%L [%Y-%m-%d %H:%M:%S.%f Δt=%iμs](%t) %^%v%$ [%s:%#]");
#endif

	slog(info, "application started");

	auto api   = api_controller{};
	auto files = file_controller{ doc_root };

	auto router = std::make_shared<request_router>();
	router->add_route(boost::regex{ R"~(^api/)~" }, api.router());
	router->add_route(boost::regex{ R"~(^files/)~" }, files.router());

	// The io_context is required for all I/O
	auto ioc = boost::asio::io_context{ threads };

	// Create and launch a listening port
	auto ec = error_code{};
	listener::run(ioc, address, port, router, ec);
	if (ec)
	{
		// errors are already logged by listener, no need to repeat that here.
		return EXIT_FAILURE;
	}

	// Capture SIGINT and SIGTERM to perform a clean shutdown
	auto signals = boost::asio::signal_set{ ioc, SIGINT, SIGTERM };
	signals.async_wait([&](const auto&, int sig) {
		// Stop the `io_context`. This will cause `run()`
		// to return immediately, eventually destroying the
		// `io_context` and all of the sockets in it.
		slog(info, "caught signal {}", sig);
		ioc.stop();
	});

	// Run the I/O service on the requested number of threads
	auto v = std::vector<std::thread>{};
	v.reserve(threads - 1);
	for (auto i = threads - 1; i > 0; --i)
	{
		v.emplace_back([&ioc] { ioc.run(); });
	}
	ioc.run();

	// (If we get here, it means we got a SIGINT or SIGTERM)

	// Block until all the threads exit
	for (auto& t : v)
	{
		t.join();
	}

	return EXIT_SUCCESS;
}
