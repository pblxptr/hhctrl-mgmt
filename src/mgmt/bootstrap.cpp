#include "bootstrap.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <zmq.hpp>

#include <mgmt/board_ctrl/board_ctrl.hpp>
#include <mgmt/board_ctrl/board_ctrl_client.hpp>

namespace {
  constexpr auto BoardControlServerAddress = "tcp://127.0.0.1:9595";
}

using boost::asio::use_awaitable;

namespace mgmt {
void bootstrap()
{
  auto handle_coroutine = [](auto eptr)
  {
    if (eptr) {
      std::rethrow_exception(eptr);
    }
  };

  //Messaging services
  auto bctx = boost::asio::io_context{};
  auto zctx = zmq::context_t{};

  //Board Controller
  auto board_ctrl_client = mgmt::board_ctrl::BoardControlClient{bctx, zctx};
  auto board_ctrl = mgmt::board_ctrl::BoardController{board_ctrl_client};
  co_spawn(bctx, board_ctrl.connect(BoardControlServerAddress), handle_coroutine);

  bctx.run();
}
}

void run()
{
}