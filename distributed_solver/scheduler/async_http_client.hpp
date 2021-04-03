#pragma once
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#include <boost/thread/future.hpp>
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <string>
#include <variant>
#include <system_error>

namespace impl
{
    class Session : public std::enable_shared_from_this<Session>
    {
        boost::promise<std::variant<std::error_code, std::string>> promise_;
        boost::asio::ip::tcp::resolver resolver_;
        boost::beast::tcp_stream stream_;
        boost::beast::flat_buffer buffer_;
        boost::beast::http::request<boost::beast::http::string_body> req_;
        boost::beast::http::response<boost::beast::http::string_body> res_;

    public:
        explicit Session(boost::asio::io_service& ios)
            : resolver_(boost::asio::make_strand(ios))
            , stream_(boost::asio::make_strand(ios))
        {
        }

        boost::unique_future<std::variant<std::error_code, std::string>>
            run(const std::string& host,
                const std::string& port,
                const std::string& target,
                const std::string& body)
        {
            req_.version(11);
            req_.method(boost::beast::http::verb::post);
            req_.target(target);
            req_.set(boost::beast::http::field::host, host);
            req_.body() = body;
            req_.prepare_payload();

            resolver_.async_resolve(host, port,
                boost::beast::bind_front_handler(
                    &Session::on_resolve,
                    shared_from_this()));

            return promise_.get_future();
        }

        void on_resolve(boost::beast::error_code ec,
            boost::asio::ip::tcp::resolver::results_type results)
        {
            if (ec)
            {
                promise_.set_value(ec);
                return;
            }

            stream_.expires_after(std::chrono::seconds(30));

            stream_.async_connect(
                results,
                boost::beast::bind_front_handler(
                    &Session::on_connect,
                    shared_from_this()));
        }

        void on_connect(boost::beast::error_code ec,
            boost::asio::ip::tcp::resolver::results_type::endpoint_type)
        {
            if (ec)
            {
                promise_.set_value(ec);
                return;
            }

            stream_.expires_after(std::chrono::seconds(30));

            boost::beast::http::async_write(stream_, req_,
                boost::beast::bind_front_handler(
                    &Session::on_write,
                    shared_from_this()));
        }

        void on_write(boost::beast::error_code ec,
            std::size_t bytes_transferred)
        {
            if (ec)
            {
                promise_.set_value(ec);
                return;
            }

            boost::beast::http::async_read(stream_, buffer_, res_,
                boost::beast::bind_front_handler(
                    &Session::on_read,
                    shared_from_this()));
        }

        void on_read(boost::beast::error_code ec,
            std::size_t bytes_transferred)
        {
            if (ec)
            {
                promise_.set_value(ec);
                return;
            }

            promise_.set_value(res_.body());

            stream_.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        }
    };
}

boost::unique_future<std::variant<std::error_code, std::string>>
AsyncHttpRequest(boost::asio::io_service& ios,
    const std::string& host, const std::string& port,
    const std::string& target, const std::string& body)
{
    return std::make_shared<impl::Session>(ios)->run(host, port, target, body);
}