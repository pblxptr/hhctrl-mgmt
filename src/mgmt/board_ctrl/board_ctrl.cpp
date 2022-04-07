#include <mgmt/board_ctrl/board_ctrl.hpp>
#include <mgmt/board_ctrl/board_info.hpp>
#include <mgmt/command/restart_board.hpp>
namespace {

  void setup_command_handlers(common::command::AsyncCommandDispatcher& command_dispatcher, mgmt::board_ctrl::BoardControlClient& client)
  {
    command_dispatcher.add_handler<mgmt::command::RestartBoard>(
        [&client](const mgmt::command::RestartBoard&) -> boost::asio::awaitable<void> {
        spdlog::get("mgmt")->info("RestartBoard execute()");

        co_await client.async_restart();
    });
  }

  bool is_board_info_valid(const mgmt::board_ctrl::BoardInfo& board_info)
  {
    return not board_info.model.empty() &&
      not board_info.hardware_revision.empty() &&
      not board_info.serial_number.empty();
  }
}

namespace mgmt::board_ctrl
{
  boost::asio::awaitable<void> async_run(
    common::command::AsyncCommandDispatcher& command_dispatcher,
    BoardControlClient& client,
    const Settings& settings
  )
  {
    //Connect
    if (const auto connected = co_await client.async_connect(settings.server_address.c_str()); not connected) {
      throw std::runtime_error("Cannot connect to main board");
    }

    //Fetch board info
    if (const auto board_info = co_await client.async_board_info(); not is_board_info_valid(board_info)) {
      throw std::runtime_error("Invalid board info");
    }
    else {
      spdlog::get("mgmt")->debug("Received board info -> model: {}, hardware revision: {}, serial number: {}",
        board_info.model,
        board_info.hardware_revision,
        board_info.serial_number
      );
    }


    //publish event
  }
}