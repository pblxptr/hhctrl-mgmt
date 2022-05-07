#include "board_ctrl_server.hpp"

#include <icon/endpoint/endpoint_config.hpp>

#include <common/utils/enum_mapper.hpp>
#include <common/data/indicator.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace {
}

using namespace boost::asio;

namespace hw::board_ctrl
{
BoardControlServer::BoardControlServer(
  boost::asio::io_context& bctx,
  zmq::context_t& zctx,
  std::string address
)
{
  spdlog::get("hw")->info("BoardControlServer: ctor, endpoint will be set on addr: {}", address);

  endpoint_ = icon::setup_default_endpoint(
    icon::use_services(bctx, zctx),
    icon::address(address),
    icon::consumer<bci::GetBoardInfoReq>([this](auto& context) -> awaitable<void> { co_await handle(context); }),
    icon::consumer<bci::RestartBoardFwd>([this](auto& context) -> awaitable<void>  { co_await handle(context); }))
  .build();

  spdlog::get("hw")->debug("BoardControlServer: ctor, endpoint built");
}

awaitable<void> BoardControlServer::async_run()
{
  spdlog::get("hw")->debug("BoardControlServer: run");

  co_await endpoint_->run();

  spdlog::get("hw")->info("BoardControlServer: run finished");
}


awaitable<void> BoardControlServer::handle(icon::MessageContext<bci::GetBoardInfoReq>& context)
{
  spdlog::get("hw")->debug("BoardControlServer: handle GetBoardInfoReq");

  auto response = bci::GetBoardInfoCfm{};
  response.set_model("Dummy_Model");
  response.set_hardware_revision("Dummy_Rev");
  response.set_serial_number("Dummy_Serial");

  co_await context.async_respond(std::move(response));
}

awaitable<void> BoardControlServer::handle(icon::MessageContext<bci::RestartBoardFwd>& context)
{
  spdlog::get("hw")->debug("BoardControlServer: handle RestartBoardFwd");

  co_return;
}

}