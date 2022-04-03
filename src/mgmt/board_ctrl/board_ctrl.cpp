#include <mgmt/board_ctrl/board_ctrl.hpp>


namespace {
  auto restartBoardCommandHandler = std::optional<RestartBoardCommandHandler>{};
}

namespace mgmt::board_ctrl
{
  boost::asio::awaitable<void> async_run(BoardControlClient& client, const std::string& bc_server_address)
  {
    restartBoardCommandHandler.emplace<RestartBoardCommandHandler>(client);


    dispatcher.register_handler<RestartBoard>(RestartBoardCommandHandler);



  }
}