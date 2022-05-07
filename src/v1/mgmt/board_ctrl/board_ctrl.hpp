#pragma once

#include <boost/asio/awaitable.hpp>

#include <common/command/dispatcher.hpp>
#include <common/utils/client.hpp>

namespace mgmt::board_ctrl
{
  boost::asio::awaitable<void> async_run(common::utils::Client);
}