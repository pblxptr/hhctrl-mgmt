#include <mgmt/board_ctrl/board.hpp>

#include <iconnect/bci/bci.pb.h>

namespace {
  template<class ExpectedMessage, class Response>
  bool response_valid(const Response& response)
  {
    return not response.error_code() || response.template is<ExpectedMessage>();
  }
}

namespace mgmt::board_ctrl
{
  MainBoardDevice::MainBoardDevice(common::utils::Client client)
    : BoardDevice{"board_device_default"}
    , client_{std::move(client)}
  {}

  boost::asio::awaitable<void> MainBoardDevice::async_restart()
  {
    co_return;
  }

  boost::asio::awaitable<mgmt::device::BoardInfo> MainBoardDevice::async_board_info()
  {
    spdlog::get("mgmt")->debug("MainBoardDevice: async_board_info");

    auto response = co_await client_.async_send(bci::GetBoardInfoReq{});
    if (not response_valid<bci::GetBoardInfoCfm>(response)) {
      spdlog::get("mgmt")->error("BoardControlClient: received invalid response messsage");
      co_return mgmt::device::BoardInfo{};
    }

    const auto& message = response.get_safe<bci::GetBoardInfoCfm>();

    co_return mgmt::device::BoardInfo {
      .model = message.model(),
      .hardware_revision = message.hardware_revision(),
      .serial_number = message.serial_number()
    };
  }
}