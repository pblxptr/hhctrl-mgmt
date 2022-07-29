#pragma once

#include <icon/client/basic_client.hpp>

namespace common::icon {
class Client : public icon::BasicClient
{
  // public:
  //   Client(
  //     zmq::context_t& zctx,
  //     boost::asio::io_context& bctx,
  //     std::string endpoint_address
  //   )
  //     : icon::BasicClient{zctx, bctx}
  //     , endpoint_addresss_{std::move(endpoint_address)}
  //   {}

  //   template<class Message>
  //   auto async_send(Message&& message)
  //   {
  //     if (not is_connected()) {
  //       co_await async_connect(endpoint_addresss_.c_str());
  //     }

  //     co_await async_send(std::forward<Message>(message));
  //   }
  // private:
  //   const std::string endpoint_addresss_;
};
}// namespace common::icon