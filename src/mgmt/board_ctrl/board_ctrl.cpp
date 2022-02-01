#include "board_ctrl.hpp"

#include <fmt/format.h>

namespace mgmt::board_ctrl {

BoardController::BoardController(mgmt::board_ctrl::BoardControlClient& client)
  : client_{client}
{}

boost::asio::awaitable<void> BoardController::connect(const std::string& addr)
{
  if (client_.is_connected()) {
    co_return;
  }

  if (!co_await client_.async_connect(addr.c_str())) {
    throw std::runtime_error(fmt::format("Cannot connect to board control client on addres: {}", addr));
  }

  co_await setup_default();
}

boost::asio::awaitable<void> BoardController::setup_default()
{
  co_await client_.set_visual_indication(common::data::IndicatorType::Status, common::data::IndicatorState::SteadyOn);
  co_await client_.set_visual_indication(common::data::IndicatorType::Warning, common::data::IndicatorState::SteadyOff);
  co_await client_.set_visual_indication(common::data::IndicatorType::Maintenance, common::data::IndicatorState::SteadyOff);
  co_await client_.set_visual_indication(common::data::IndicatorType::Fault, common::data::IndicatorState::SteadyOff);
}

}
