#pragma once

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <icon/endpoint/endpoint.hpp>
#include <icon/endpoint/message_context.hpp>
#include <iconnect/bci/bci.pb.h>

#include <string>

namespace hw::board_ctrl
{
  class BoardControlServer
  {
  public:
    explicit BoardControlServer(
      boost::asio::io_context&,
      zmq::context_t&,
      std::string
    );
    boost::asio::awaitable<void> run();
  private:
    boost::asio::awaitable<void> handle(icon::MessageContext<bci::GetBoardInfoReq>&);
    boost::asio::awaitable<void> handle(icon::MessageContext<bci::RestartBoardFwd>&);

  private:
    std::unique_ptr<icon::Endpoint> endpoint_{};

  };
}
