#include "bootstrap.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <zmq.hpp>

#include <mgmt/board_ctrl/board_ctrl.hpp>
#include <mgmt/board_ctrl/board_ctrl_client.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace {
  constexpr auto BoardControlServerAddress = "tcp://127.0.0.1:9595";
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

  auto handle_coroutine = [](auto eptr)
  {
    if (eptr) {
      std::rethrow_exception(eptr);
    }
  };

  //Messaging services
  auto bctx = boost::asio::io_context{};
  auto zctx = zmq::context_t{};
  work_guard_type work_guard(bctx.get_executor());

  //Board Controller
  auto board_ctrl_client = mgmt::board_ctrl::BoardControlClient{bctx, zctx};
  auto board_ctrl = mgmt::board_ctrl::BoardController{board_ctrl_client};

  auto addr = std::string{BoardControlServerAddress};

  boost::asio::co_spawn(bctx, board_ctrl.connect(addr), handle_coroutine);

  bctx.run();
}
}
