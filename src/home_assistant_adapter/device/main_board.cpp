#include <home_assistant/device/main_board.hpp>
#include <coro/co_spawn.hpp>

namespace mgmt::home_assistant::device
{
    boost::asio::awaitable<std::optional<MainBoard>> MainBoard::async_create(
        mgmt::device::DeviceId_t device_id,
        const adapter::EntityFactory& factory,
        const DeviceIdentityProvider& identity_provider
    )
    {
        auto identity = identity_provider.identity(device_id);

        auto fault_indicator = co_await detail::MainBoardIndicator::async_create(
                device_id,
                mgmt::device::IndicatorType::Fault,
                factory,
                identity
        );
        auto status_indicator = co_await detail::MainBoardIndicator::async_create(
                device_id,
                mgmt::device::IndicatorType::Status,
                factory,
                identity
        );
        auto warning_indicator = co_await detail::MainBoardIndicator::async_create(
                device_id,
                mgmt::device::IndicatorType::Warning,
                factory,
                identity
        );
        auto maintenance_indicator = co_await detail::MainBoardIndicator::async_create(
                device_id,
                mgmt::device::IndicatorType::Maintenance,
                factory,
                identity
        );

        if (!fault_indicator || !status_indicator || !warning_indicator || !maintenance_indicator) {
            co_return std::nullopt;
        }
        auto indicators = IndicatorMap_t {};
        indicators.emplace(mgmt::device::IndicatorType::Fault, std::move(*fault_indicator));
        indicators.emplace(mgmt::device::IndicatorType::Status, std::move(*status_indicator));
        indicators.emplace(mgmt::device::IndicatorType::Warning, std::move(*warning_indicator));
        indicators.emplace(mgmt::device::IndicatorType::Maintenance, std::move(*maintenance_indicator));

        co_return MainBoard{std::move(indicators)};
    }

    MainBoard::MainBoard(IndicatorMap_t indicators)
        : indicators_{std::move(indicators)}
    {}

    boost::asio::awaitable<void> MainBoard::async_run()
    {
        auto executor = co_await boost::asio::this_coro::executor;

        // Spawn coroutines for indicators
        for (auto&& [k, indicator] : indicators_) {
            boost::asio::co_spawn(executor, indicator.async_run(), common::coro::rethrow);
        }
    }

    boost::asio::awaitable<void> MainBoard::async_sync_state()
    {
        // Sync indicators
        for (auto&& [k, indicator] : indicators_) {
            co_await indicator.async_sync_state();
        }
    }
}
