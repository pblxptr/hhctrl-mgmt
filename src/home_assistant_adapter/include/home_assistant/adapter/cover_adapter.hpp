#pragma once

#include <utility>

#include <home_assistant/mqtt/coverv2.hpp>
#include <home_assistant/adapter/logger.hpp>
#include <coro/async_wait.hpp>

namespace mgmt::home_assistant::adapter {

template <typename Impl, typename CoverEntity>
class CoverAdapter
{
public:
  // movable
  CoverAdapter(CoverAdapter&& rhs) noexcept = default;
  CoverAdapter& operator=(CoverAdapter&& rhs) noexcept = default;
  // non-copyable
  CoverAdapter(const CoverAdapter&) = delete;
  CoverAdapter& operator=(const CoverAdapter&) = delete;

  ~CoverAdapter() = default;

  boost::asio::awaitable<bool> async_init()
  {
      const auto connected = co_await async_connect();
      if (!connected) {
          co_return true;
      }

      const auto configured = co_await async_configure();
      if (!configured) {
          co_return false;
      }
  }

  boost::asio::awaitable<void> async_run()
  {

  }

  boost::asio::awaitable<void> async_sync_state();

private:
  explicit CoverAdapter(CoverEntity cover);

  boost::asio::awaitable<bool> async_connect()
  {
      common::logger::get(Logger)->trace("CoverAdapter::{}", __FUNCTION__);

      const auto error = co_await cover_.async_connect();
      if (error) {
          common::logger::get(Logger)->error("Cannot establish connection for cover entity with unique id: '{}'", cover_.unique_id());
          co_return false;
      }

      co_return true;
  }

  boost::asio::awaitable<bool> async_configure()
  {
      common::logger::get(Logger)->trace("CoverAdapter::{}", __FUNCTION__);
      // Configure
      {
          auto config = impl().config();
          config.set("name", "Cover");
          config.set("device_class", "door");

          const auto error = co_await cover_.async_configure();

          if (error) {
              common::logger::get(Logger)->error("Cannot configure cover entity with unique id: '{}'", cover_.unique_id());
              co_return false;
          }
      }

      // Wait until entity is configured on remote
      co_await common::coro::async_wait(std::chrono::seconds(1));

      co_await cover_.async_set_availability(v2::Availability::Online);
      co_await async_sync_state();

      co_return true;
  }

  boost::asio::awaitable<void> async_handle_command(const v2::CoverCommand& command)
  {
      if (std::holds_alternative<v2::CoverTiltCommand>(command)) {
          auto value = std::get<v2::CoverTiltCommand>(command);
          impl().async_handle_command(value);
      }
      else if (std::holds_alternative<v2::CoverSwitchCommand>(command)) {
          const auto& value = std::get<v2::CoverSwitchCommand>(command);
          impl().async_handle_command(value);
      }
      else {
          common::logger::get(Logger)->error("Command not supported: '{}'", cover_.unique_id());
      }
  }

  boost::asio::awaitable<bool> async_handle_disconnected_error()
  {

  }

  boost::asio::awaitable<bool> async_handle_reconnected_error()
  {

  }

  Impl& impl()
  {
      return static_cast<Impl&>(*this);
  }

private:
  CoverEntity cover_;
};
}// namespace mgmt::home_assistant::adapter

