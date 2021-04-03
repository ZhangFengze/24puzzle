#pragma once
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#include "boost/thread/future.hpp"
#include "boost/asio.hpp"
#include <string>
#include <variant>
#include <system_error>

boost::unique_future<std::variant<std::error_code, std::string>>
AsyncHttpRequest(boost::asio::io_service& ios,
    const std::string& host, const std::string& port,
    const std::string& target, const std::string& body);