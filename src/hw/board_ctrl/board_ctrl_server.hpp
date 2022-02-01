#pragma once

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <icon/endpoint/endpoint.hpp>
#include <icon/endpoint/message_context.hpp>
#include <bci.pb.h>
#include <hw/services/led_service.hpp>

#include <string>

namespace hw::board_ctrl
{
  class BoardControlServer
  {
  public:
    explicit BoardControlServer(
      hw::services::LedService&,
      boost::asio::io_context&,
      zmq::context_t&,
      std::string
    );
    boost::asio::awaitable<void> run();
  private:
    boost::asio::awaitable<void> handle(icon::MessageContext<bci::GetVisualIndicationReq>&);
    boost::asio::awaitable<void> handle(icon::MessageContext<bci::SetVisualIndicationReq>&);
    boost::asio::awaitable<void> handle(icon::MessageContext<bci::RestartBoardFwd>&);

  private:
    hw::services::LedService& led_service_;
    std::unique_ptr<icon::Endpoint> endpoint_{};

  };
}
