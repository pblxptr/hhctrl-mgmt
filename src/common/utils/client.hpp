#pragma once

#include <icon/client/basic_client.hpp>
#include <type_traits>

namespace common::utils {
class Client : protected icon::BasicClient
{
public:
  using BasicClient::Response_t;

  Client(
    boost::asio::io_context& bctx,
    zmq::context_t& zctx,
    std::string endpoint_address)
    : icon::BasicClient{ zctx, bctx }, endpoint_addresss_{ std::move(endpoint_address) }
  {
    spdlog::get("mgmt")->debug("Client: ctor");
  }

  ~Client()
  {
    spdlog::get("mgmt")->debug("Client: dtor");
  }

  Client(const Client&&) = delete;
  Client(Client&& src) : icon::BasicClient(std::move(src))
  {
    spdlog::get("mgmt")->debug("Client: move ctor");
    endpoint_addresss_ = std::move(src.endpoint_addresss_);
  }

  template<class Message, class Timeout = std::chrono::seconds>
  boost::asio::awaitable<Response_t> async_send(Message&& message, Timeout timeout = Timeout{ 0 })
  {
    spdlog::get("mgmt")->debug("Address: {}", endpoint_addresss_);

    if (not BasicClient::is_connected()) {
      spdlog::get("mgmt")->debug("Client: async_send - connecting");
      co_await BasicClient::async_connect(endpoint_addresss_.c_str());
    }

    co_return co_await BasicClient::async_send(std::forward<Message>(message), timeout);
  }

private:
  std::string endpoint_addresss_;
};
}// namespace common::utils