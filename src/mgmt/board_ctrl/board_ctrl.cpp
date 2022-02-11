#include "board_ctrl.hpp"

#include <fmt/format.h>
#include <iostream>

namespace mgmt::board_ctrl {

BoardController::BoardController(mgmt::board_ctrl::BoardControlClient& client)
  : client_{client}
{
  spdlog::get("mgmt")->info("BoardController ctor");
}

boost::asio::awaitable<void> BoardController::connect(const std::string& str)
{
  spdlog::get("mgmt")->info("BoardController: connect: {}", str);

  if (client_.is_connected()) {
    spdlog::get("mgmt")->info("BoardController: already connected");

    co_return;
  }

  if (const auto connected = co_await client_.async_connect(str.c_str()); !connected) {
    throw std::runtime_error(fmt::format("Cannot connect to board control client on addres: {}", str));
  }

  co_await setup_default();
}

boost::asio::awaitable<void> BoardController::setup_default()
{
  spdlog::get("mgmt")->info("BoardController: setup_default");

  co_await client_.set_visual_indication(common::data::IndicatorType::Status, common::data::IndicatorState::SteadyOff);
  co_await client_.set_visual_indication(common::data::IndicatorType::Warning, common::data::IndicatorState::SteadyOff);
  co_await client_.set_visual_indication(common::data::IndicatorType::Maintenance, common::data::IndicatorState::SteadyOff);
  co_await client_.set_visual_indication(common::data::IndicatorType::Fault, common::data::IndicatorState::SteadyOff);

  co_await client_.set_visual_indication(common::data::IndicatorType::Status, common::data::IndicatorState::SteadyOn);

  spdlog::get("mgmt")->info("BoardController: default setup done");
}

}
