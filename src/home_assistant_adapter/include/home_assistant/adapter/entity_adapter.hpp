#pragma once

#include <boost/asio/awaitable.hpp>
#include <home_assistant/adapter/logger.hpp>
#include <home_assistant/mqtt/availability.hpp>
#include <home_assistant/mqtt/error.hpp>
#include <home_assistant/mqtt/expected.hpp>
#include <home_assistant/mqtt/opts.hpp>
#include <home_assistant/mqtt/will.hpp>
#include <home_assistant/mqtt/entity.hpp>
#include <coro/async_wait.hpp>

namespace detail {
    template <typename T>
    concept Synchronizable = requires(T obj)
    {
        { obj.state() };
    };

    constexpr inline auto AdapterDefaultPubopts = mgmt::home_assistant::mqtt::Retain_t::yes | mgmt::home_assistant::mqtt::Qos_t::at_least_once;
    constexpr inline auto InitRetryAttempts = 10;
    constexpr inline auto InitRetryDelay = std::chrono::seconds{60};
} // namespace detail

namespace mgmt::home_assistant::adapter
{
    template <typename Entity, typename Impl>
    class EntityAdapter
    {
    public:
        EntityAdapter(Entity entity)
            : entity_{std::move(entity)}
        {
            entity_.set_will(mqtt::WillConfig{
                .topic = entity_.topic(mqtt::GenericEntityConfig::AvailabilityTopic),
                .message = std::string{mqtt::AvailabilityStateMapper.map(mqtt::Availability::Offline)},
                .pubopts = mqtt::Retain_t::yes
                }
            );
        }

        const std::string& unique_id() const
        {
            return entity_.unique_id();
        }

        boost::asio::awaitable<bool> async_init()
        {
            common::logger::get(Logger)->trace("EntityAdapter::{}", __FUNCTION__);

            int attempt = 0;
            auto error = mqtt::Error{};

            while (attempt++ < detail::InitRetryAttempts) {
                error = co_await async_connect();
                if (!error) {
                    break;
                }
                common::logger::get(Logger)->warn("EntityAdapter init error: {}, retry: {}/{}, delay: {}s",
                    error.what(), attempt, detail::InitRetryAttempts, detail::InitRetryDelay.count()
                );
                co_await common::coro::async_wait(detail::InitRetryDelay);
            }

            if (error) {
                co_return false;
            }

            const auto configured = co_await async_configure();
            if (!configured) {
                co_return false;
            }

            co_return true;
        }

        boost::asio::awaitable<void> async_sync_state() requires detail::Synchronizable<Impl>
        {
            common::logger::get(Logger)->trace("EntityAdapter::{}", __FUNCTION__);

            auto state = impl().state();

            const auto error = co_await entity_.async_set_state(std::move(state), detail::AdapterDefaultPubopts);

            if (error) {
                common::logger::get(Logger)->error("Error while syncing state: {}", error.what());
            }
        }

        boost::asio::awaitable<void> async_sync_state() requires (!detail::Synchronizable<Impl>)
        {
            common::logger::get(Logger)->debug("EntityAdapter::{}, empty impl", __FUNCTION__);

            co_return;
        }

        boost::asio::awaitable<void> async_run()
        {
            while (true) {
                auto value = co_await entity_.async_receive();

                using ValueType = decltype(value);

                if constexpr (std::is_same_v<mqtt::Error, ValueType>) {
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

        boost::asio::awaitable<mqtt::Error> async_connect()
        {
            common::logger::get(Logger)->trace("EntityAdapter::{}", __FUNCTION__);

            const auto error = co_await entity_.async_connect();
            if (error) {
                common::logger::get(adapter::Logger)->error("Cannot establish connection for cover entity with unique id: '{}'", entity_.unique_id());
            }

            co_return error;
        }

        boost::asio::awaitable<bool> async_configure()
        {
            common::logger::get(Logger)->trace("EntityAdapter::{}", __FUNCTION__);
            // Configure
            {
                auto config = impl().config();

                const auto error = co_await entity_.async_configure(std::move(config), detail::AdapterDefaultPubopts);

                if (error) {
                    common::logger::get(Logger)->error("Cannot configure cover entity with unique id: '{}'", entity_.unique_id());
                    co_return false;
                }
            }

            // Wait until entity is configured on remote
            co_await common::coro::async_wait(std::chrono::seconds(1));

            co_await entity_.async_set_availability(mqtt::Availability::Online, detail::AdapterDefaultPubopts);
            co_await async_sync_state();

            common::logger::get(Logger)->debug("Entity with unique id: '{}' configured properly.", entity_.unique_id());

            co_return true;
        }

        boost::asio::awaitable<void> async_handle_error(const mqtt::Error& error)
        {
            bool recovered = false;

            common::logger::get(Logger)->error("Handling error for: {}", entity_.unique_id());
            auto err = mqtt::Error{mqtt::ErrorCode::Reconnected, "Client got reconnected"};

            if (error.code() == mqtt::ErrorCode::Disconnected) {
                recovered = co_await async_handle_disconnected_error();
            }
            else if (error.code() == mqtt::ErrorCode::Reconnected) {
                recovered = co_await async_handle_reconnected_error();
            }
            else {
                common::logger::get(Logger)->error("EntityAdapter could not recognize error - num: {}, what: ", error.code().value(), error.what());
            }

            if (!recovered) {
                common::logger::get(Logger)->error("EntityAdapter could not recover from error: '{}', for entity with unique id: '{}'", error.what(), entity_.unique_id());
            }
            else {
                common::logger::get(Logger)->error("EntityAdapter has recovered from error: '{}', for entity with unique id: '{}'", error.what(), entity_.unique_id());
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
