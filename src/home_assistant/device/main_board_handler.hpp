#pragma once

namespace mgmt::home_assistant::device
{
  template<
    class Switch,
    class BinarySensor
  >
  class MainBoardHandler
  {
  public:
    MainBoardHandler(
      Switch restart_switch,
      std::unordered_map<mgmt::device::IndicatorType, BinarySensor> indicators
    )
      : restart_switch_{std::move(restart_switch)}
      , indicators_{std::move(indicators)}
    {
      spdlog::debug("MainBoardHandler::{}", __FUNCTION__);

      setup();
    }

    mgmt::device::DeviceId_t hardware_id() const
    {
      return id_;
    }

    void connect()
    {
      spdlog::debug("MainBoardHandler::{}", __FUNCTION__);

      //Connect switch
      restart_switch_.connect();

      //Connect indicators
      for (auto&& [k, v] : indicators_) {
        v.connect();
      }
    }

    void async_sync_state()
    {
      spdlog::debug("MainBoardHandler::{}", __FUNCTION__);

      const auto& main_board = mgmt::device::inventory<mgmt::device::MainBoard>.get(id_);

      for (auto&& [type,sensor] : indicators_) {
        auto state = main_board.indicator_state(type);
        sensor.async_set_state(map_state(state));
      }
    }

  private:
    void setup()
    {
      spdlog::debug("MainBoardHandler::{}", __FUNCTION__);

      //Setup switch
      restart_switch_.set_ack_handler([this]() { set_config_switch(); });
      restart_switch_.set_close_handler([this]() { on_close(); });
      restart_switch_.set_error_handler([this]() { on_error(); });
      restart_switch_.on_command([this]() {
        auto& main_board = mgmt::device::inventory<mgmt::device::MainBoard>.get(id_);
        main_board.restart();
      });

      //Setup indicators
      for (auto&& [type, sensor] : indicators_) {
        sensor.set_ack_handller([this, type]() { set_config_indicator(type); });
        sensor.set_close_handler([this]() { on_close(); });
        sensor.set_error_handller([this]() { on_error(); });
      }
    }

    void set_config_switch()
    {
      spdlog::debug("MainBoardHandler::{}", __FUNCTION__);
    }

    void set_config_indicator(const mgmt::device::IndicatorType& type)
    {
      spdlog::debug("MainBoardHandler::{}", __FUNCTION__);
    }

    void on_close()
    {
      spdlog::debug("MainBoardHandler::{}", __FUNCTION__);
    }

    void on_error()
    {
      spdlog::debug("MainBoardHandler::{}", __FUNCTION__);
    }

  private:
    mgmt::device::DeviceId_t id_;
    Switch restart_switch_;
    std::unordered_map<mgmt::device::IndicatorType, BinarySensor> indicators_;
  };
}