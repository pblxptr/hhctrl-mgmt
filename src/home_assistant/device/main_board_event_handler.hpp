#pragma once

#include <home_assistant/entity_factory.hpp>
#include <events/device_created.hpp>

namespace mgmt::home_assistant::device {
  class MainBoardEventHandler
  {
    using DeviceCreated_t = mgmt::event::DeviceCreated<mgmt::device::MainBoard>;
    using DeviceRemoved_t = mgmt::event::DeviceRemoved<mgmt::device::MainBoard>;
    using DeviceStateChanged_t = mgmt::event::DeviceStateChanged<mgmt::device::MainBoard>;
  public:
    explicit MainBoardEventHandler(const EntityFactory& factory)
      : factory_{factory}
    {
      spdlog::debug("MainBoardEventHandler::{}", __FUNCTION__);
    }

    boost::asio::awaitable<void> operator()(const DeviceCreated_t& event)
    {
      spdlog::debug("MainBoardEventHandler::{}", __FUNCTION__);

      // const auto& board = mgmt::device::inventory<mgmt::device::MainBoard>.get(event.device_id);
      // auto unique_id = event.device_id + "main_board";

      // auto board_handler = MainBoardHandler {
      //   factory_.create_switch("unique_id_switch"),
      //   {
      //     std::pair { mgmt::device::IndicatorType::Fault, factory_.create_binary_switch("unique_id_binary_sensor")},
      //   }
      //   std::move(indicators)
      // };

      // board_handler.on_error([this]() { on_error(); });
      // board_handler.connect();

      co_return;
    }

    boost::asio::awaitable<void> operator()(const DeviceRemoved_t& event)
    {
      spdlog::debug("MainBoardEventHandler::{}", __FUNCTION__);

      co_return;
    }

    boost::asio::awaitable<void> operator()(const DeviceStateChanged_t& event)
    {
      spdlog::debug("MainBoardEventHandler::{}", __FUNCTION__);

      co_return;
    }
  private:
    void on_error()
    {
      spdlog::debug("MainBoardEventHandler::{}", __FUNCTION__);
    }

    void on_close()
    {
      spdlog::debug("MainBoardEventHandler::{}", __FUNCTION__);
    }

  private:
    EntityFactory factory_;
    // std::vector<MainBoardHandler>
  };

}

