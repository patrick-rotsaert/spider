#include "spider/controller.hpp"

namespace spider {

controller::exception_handler_base::~exception_handler_base()
{
}

controller::argument_error::argument_error(const std::string& m)
    : exception_base{ m }
{
}

controller::~controller()
{
}

controller::controller(std::shared_ptr<request_router> router)
    : router_{ std::move(router) }
    , exception_handler_{}
{
}

const std::shared_ptr<request_router>& controller::router() const
{
	return this->router_;
}

void controller::exception_handler(const std::shared_ptr<exception_handler_base>& value)
{
	this->exception_handler_ = value;
}

} // namespace spider
