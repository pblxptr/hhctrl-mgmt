#pragma once

#include <boost/asio/awaitable.hpp>
#include <home_assistant/device_identity_provider.hpp>
#include <home_assistant/adapter/entity_factory.hpp>
#include <home_assistant/adapter/logger.hpp>
#include <coro/async_wait.hpp>

namespace mgmt::home_assistant::adapter
{
    template <typename Impl>
    class IndicatorHandler
    {
    protected:
        explicit IndicatorHandler(BinarySensor_t binary_sensor)
            : binary_sensor_{std::move(binary_sensor)}
        {}

        Impl& impl()
        {
            return static_cast<Impl&>(*this);
        }

    public:
        boost::asio::awaitable<bool> async_init()
        {
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
            co_await binary_sensor_.async_set_state(impl().state());
        }

        boost::asio::awaitable<void> async_run()
        {
            common::logger::get(Logger)->trace("IndicatorHandler::{}", __FUNCTION__);

//            while (true) {
//                const auto  = co_await cover_.async_receive();
//
//                if (command) {
//                    co_await async_handle_command(command.value());
//                }
//                else {
//                    bool recovered = false;
//
//                    if (command.error().code() == v2::ErrorCode::Disconnected) {
//                        recovered = co_await async_handle_disconnected_error();
//                    }
//                    else if (command.error().code() == v2::ErrorCode::Reconnected) {
//                        recovered = co_await async_handle_reconnected_error();
//                    }
//                    else {
//                        common::logger::get(mgmt::home_assistant::adapter::Logger)->error("IndicatorHandler unhandled error: '{}'", command.error().what());
//                    }
//
//                    if (!recovered) {
//                        common::logger::get(mgmt::home_assistant::adapter::Logger)->trace("IndicatorHandler could not recover from error: '{}'", command.error().what());
//                    }
//                }
//            }
            co_return ;
        }


    private:
        boost::asio::awaitable<bool> async_connect()
        {
            common::logger::get(Logger)->trace("IndicatorHandler::{}", __FUNCTION__);

            const auto error = co_await binary_sensor_.async_connect();
            if (error) {
                common::logger::get(Logger)->error("Cannot establish connection for cover entity with unique id: '{}'", binary_sensor_.unique_id());
                co_return false;
            }

            co_return true;
        }

        boost::asio::awaitable<bool> async_configure()
        {
            common::logger::get(mgmt::home_assistant::adapter::Logger)->trace("IndicatorHandler::{}", __FUNCTION__);
            auto config = impl().config();
            config.set("name", "Default Indicator Name");
            config.set("device_class", "light");
            config.set("entity_category", "diagnostic");

            const auto error = co_await binary_sensor_.async_configure(std::move(config));
            if (error) {
                common::logger::get(Logger)->error("Cannot configure binary_sensor entity with unique id: '{}'", binary_sensor_.unique_id());
            }

            co_await common::coro::async_wait(std::chrono::seconds(1));

            co_await binary_sensor_.async_set_availability(v2::Availability::Online);
            co_await async_sync_state();

            co_return true;
        }

    private:
        BinarySensor_t binary_sensor_;
    };
}
