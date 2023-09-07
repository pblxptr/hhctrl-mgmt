#pragma once

#include <boost/asio/awaitable.hpp>
#include <home_assistant/adapter/logger.hpp>
#include <home_assistant/mqtt/availability2.hpp>
#include <home_assistant/mqtt/error.hpp>
#include <home_assistant/mqtt/expected.hpp>
#include <coro/async_wait.hpp>

namespace mgmt::home_assistant::adapter
{
    template <typename Entity, typename Impl>
    class EntityAdapter
    {
    public:
        EntityAdapter(Entity entity)
                : entity_{std::move(entity)}
        {}

        const std::string& unique_id() const
        {
            return entity_.unique_id();
        }

        boost::asio::awaitable<bool> async_init()
        {
            common::logger::get(Logger)->trace("EntityAdapter::{}", __FUNCTION__);

            const auto connected = co_await async_connect();
            if (!connected) {
                co_return false;
            }

            const auto configured = co_await async_configure();
            if (!configured) {
                co_return false;
            }

            co_return true;
        }

        boost::asio::awaitable<void> async_sync_state()
        {
            common::logger::get(Logger)->trace("EntityAdapter::{}", __FUNCTION__);

            auto state = impl().state();

            const auto error = co_await entity_.async_set_state(std::move(state));

            if (error) {
                common::logger::get(Logger)->error("Error while syncing state: {}", error.what());
            }
        }

        boost::asio::awaitable<void> async_run()
        {
            while (true) {
                auto value = co_await entity_.async_receive();

                using ValueType = decltype(value);

                if constexpr (std::is_same_v<v2::Error, ValueType>) {
                    co_await async_handle_error(value);
                }
                else {
                    if (value) {
                        co_await async_handle_recv_value(value);
                    }
                    else {
                        co_await async_handle_error(value.error());
                    }
                }
            }
        }

    private:
        Impl& impl()
        {
            return static_cast<Impl&>(*this);
        }

        boost::asio::awaitable<bool> async_connect()
        {
            common::logger::get(Logger)->trace("EntityAdapter::{}", __FUNCTION__);

            const auto error = co_await entity_.async_connect();
            if (error) {
                common::logger::get(adapter::Logger)->error("Cannot establish connection for cover entity with unique id: '{}'", entity_.unique_id());
                co_return false;
            }

            co_return true;
        }

        boost::asio::awaitable<bool> async_configure()
        {
            common::logger::get(Logger)->trace("EntityAdapter::{}", __FUNCTION__);
            // Configure
            {
                auto config = impl().config();

                const auto error = co_await entity_.async_configure(std::move(config));

                if (error) {
                    common::logger::get(Logger)->error("Cannot configure cover entity with unique id: '{}'", entity_.unique_id());
                    co_return false;
                }
            }

            // Wait until entity is configured on remote
            co_await common::coro::async_wait(std::chrono::seconds(1));

            co_await entity_.async_set_availability(v2::Availability::Online);
            co_await async_sync_state();

            co_return true;
        }

        boost::asio::awaitable<void> async_handle_error(const v2::Error& error)
        {
            bool recovered = false;

            const auto& error_code = error.code();

            if (error_code == v2::ErrorCode::Disconnected) {
                recovered = co_await async_handle_disconnected_error();
            }
            else if (error_code == v2::ErrorCode::Reconnected) {
                recovered = co_await async_handle_reconnected_error();
            }

            if (!recovered) {
                common::logger::get(Logger)->trace("EntityAdapter could not recover from error: '{}'", error.what());
            }
        }

        template <typename T>
        boost::asio::awaitable<void> async_handle_recv_value(T value)
        {
            co_await impl().async_handle_recv_value(value.value());
        }

        boost::asio::awaitable<bool> async_handle_disconnected_error()
        {
            common::logger::get(Logger)->trace("EntityAdapter::{}", __FUNCTION__);

            const auto error_code = co_await async_connect();

            if (error_code) {
                co_return false;
            }

            co_return co_await async_configure();
        }

        boost::asio::awaitable<bool> async_handle_reconnected_error()
        {
            common::logger::get(Logger)->trace("EntityAdapter::{}", __FUNCTION__);

            co_return co_await async_configure();
        }

    private:
        Entity entity_;
    };

}
