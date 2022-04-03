#include "bootstrap.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <zmq.hpp>

#include <common/coro/co_spawn.hpp>
#include <mgmt/board_ctrl/board_ctrl_client.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace {
  constexpr auto BoardControlServerAddress = "tcp://127.0.0.1:9595";
  constexpr auto PlatformDeviceControlServerAddress = "tcp://127.0.0.1:9596";
}

using boost::asio::use_awaitable;
using work_guard_type =
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;


namespace mgmt {
void bootstrap()
{
  static auto console_logger = spdlog::stdout_color_mt("mgmt");
  console_logger->info("Booststrap: mgmt");
  console_logger->info("Booststrap: {}", BoardControlServerAddress);

  //Messaging services
  auto bctx = boost::asio::io_context{};
  auto zctx = zmq::context_t{};
  work_guard_type work_guard(bctx.get_executor());

  //Clients
  auto bc_client = mgmt::board_ctrl::BoardControlClient{bctx, zctx};

  //Board Controller
  auto bc_addr = std::string{BoardControlServerAddress};
  auto pdctrl_addr = std::string{PlatformDeviceControlServerAddress};

  //Run
  // boost::asio::co_spawn(bctx, mgmt::board_ctrl::async_run(bc_client));

  bctx.run();
}
}
