#include "board_ctrl_client.hpp"

#include <iconnect/bci/bci.pb.h>
#include <spdlog/spdlog.h>


using boost::asio::awaitable;

namespace {
  constexpr auto RequestTimeout = std::chrono::seconds(0);

  template<class ExpectedMessage, class Response>
  bool response_valid(const Response& response)
  {
    return not response.error_code() || response.template is<ExpectedMessage>();
  }
}

namespace mgmt::board_ctrl
{
BoardControlClient::BoardControlClient(boost::asio::io_context& bctx, zmq::context_t& zctx)
  : BasicClient(zctx, bctx)
{
  spdlog::get("mgmt")->info("BoardControlClient: ctor");
}

boost::asio::awaitable<BoardInfo> BoardControlClient::async_board_info()
{
  spdlog::get("mgmt")->info("BoardControlClient: async_board_info");

  auto response = co_await async_send(bci::GetBoardInfoReq{}, RequestTimeout);
  if (not response_valid<bci::GetBoardInfoCfm>(response)) {
    spdlog::get("mgmt")->error("BoardControlClient: received invalid response messsage");
    co_return BoardInfo{};
  }

  const auto& message = response.get_safe<bci::GetBoardInfoCfm>();

  co_return BoardInfo {
    .model = message.model(),
    .hardware_revision = message.hardware_revision(),
    .serial_number = message.serial_number()
  };
}

boost::asio::awaitable<void> BoardControlClient::async_restart()
{
  spdlog::get("mgmt")->info("BoardControlClient: async_restart");

  co_await async_send(bci::RestartBoardFwd{}, RequestTimeout);
}
}