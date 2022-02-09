#include "board_ctrl_client.hpp"

#include <common/mapper/indicator_mapper_config.hpp>
#include <spdlog/spdlog.h>

using boost::asio::awaitable;

namespace {
  constexpr auto RequestTimeout = std::chrono::seconds(0);

  template<class ResponseMessage, class Response>
  void check_if_valid(const Response& response)
  {
    if (response.error_code() || !response.template is<ResponseMessage>()) {
      throw std::runtime_error("Response contains error code or invalid message.");
    }
  }
}

namespace mgmt::board_ctrl
{
BoardControlClient::BoardControlClient(boost::asio::io_context& bctx, zmq::context_t& zctx)
  : BasicClient(zctx, bctx)
{
  spdlog::get("mgmt")->info("BoardControlClient: ctor");
}

awaitable<void> BoardControlClient::set_visual_indication(
  const common::data::IndicatorType& indicator_type,
  const common::data::IndicatorState& state
)
{
  spdlog::get("mgmt")->info("BoardControlClient: set_visual_indication");

  auto req = bci::SetVisualIndicationReq{};
  req.set_indicator(common::mapper::IndicatorTypeMapper_t::map_safe<bci::IndicatorType>(indicator_type));
  req.set_state(common::mapper::IndicatorStateMapper_t::map_safe<bci::IndicatorState>(state));

  auto response = co_await async_send(std::move(req), RequestTimeout);

  check_if_valid<bci::SetVisualIndicationCfm>(response);
}

awaitable<common::data::IndicatorState> BoardControlClient::get_visual_indication(const common::data::IndicatorType& indicator_type)
{
  spdlog::get("mgmt")->info("BoardControlClient: get_visual_indication");

  auto req = bci::GetVisualIndicationReq{};
  req.set_indicator(common::mapper::IndicatorTypeMapper_t::map_safe<bci::IndicatorType>(indicator_type));

  const auto response = co_await async_send(std::move(req), RequestTimeout);
  const auto message = response.get_safe<bci::GetVisualIndicationCfm>();

  co_return common::mapper::IndicatorStateMapper_t::map_safe<common::data::IndicatorState>(message.state());
}
}
