#pragma once

#include <boost/asio/awaitable.hpp>

#include <mgmt/board_ctrl/board_ctrl_client.hpp>
#include <mgmt/board_ctrl/settings.hpp>
#include <common/command/dispatcher.hpp>

namespace mgmt::board_ctrl
{
  boost::asio::awaitable<void> async_run(
    common::command::AsyncCommandDispatcher& dispatcher,
    BoardControlClient& client,
    const Settings& settings
  );
}