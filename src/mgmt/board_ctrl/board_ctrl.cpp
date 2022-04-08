#include <mgmt/board_ctrl/board_ctrl.hpp>
#include <mgmt/device/board_device.hpp>
#include <mgmt/command/restart_board.hpp>
#include <iconnect/bci/bci.pb.h>
#include <mgmt/board_ctrl/board.hpp>
namespace {


  bool is_board_info_valid(const mgmt::device::BoardInfo& board_info)
  {
    return not board_info.model.empty() &&
      not board_info.hardware_revision.empty() &&
      not board_info.serial_number.empty();
  }
}

namespace mgmt::board_ctrl
{
  boost::asio::awaitable<void> async_run(common::utils::Client client)
  {
    auto main_board = mgmt::board_ctrl::MainBoardDevice{std::move(client)};

    auto board_info = co_await main_board.async_board_info();

    spdlog::get("mgmt")->debug("Received board info -> model: {}, hardware revision: {}, serial number: {}",
      board_info.model,
      board_info.hardware_revision,
      board_info.serial_number
    );
  }
}