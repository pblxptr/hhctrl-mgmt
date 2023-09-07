#pragma once

#include <home_assistant/adapter/entity_adapter.hpp>
#include <home_assistant/adapter/unique_id.hpp>
#include <home_assistant/adapter/logger.hpp>
#include <home_assistant/device_identity_provider.hpp>
#include <home_assistant/entity_factory.hpp>
#include <home_assistant/mqtt/coverv2.hpp>
#include <device/device_register.hpp>

namespace mgmt::home_assistant::device
{
    class Hatch : public adapter::EntityAdapter<adapter::Cover_t, Hatch>
    {
    public:
        // movable
        Hatch(Hatch&& rhs) noexcept = default;
        Hatch& operator=(Hatch&& rhs) noexcept = default;
        // non-copyable
        Hatch(const Hatch&) = delete;
        Hatch& operator=(const Hatch&) = delete;

        ~Hatch() = default;

        static boost::asio::awaitable<std::optional<Hatch>> async_create(
                mgmt::device::DeviceId_t device_id,
                const DeviceIdentityProvider& identity_provider,
                const adapter::EntityFactory& factory
        )
        {
            auto cover = factory.create_cover(adapter::get_unique_id(device_id, identity_provider.identity(device_id)));
            auto handler = Hatch{device_id, std::move(cover), identity_provider.identity(device_id)};

            const auto initialized = co_await handler.async_init();
            if (!initialized) {
                co_return std::nullopt;
            }

            co_return handler;
        }

        mgmt::device::DeviceId_t hardware_id() const
        {
            return device_id_;
        }

        v2::EntityConfig config() const
        {
            auto config = v2::EntityConfig{};
            config.set("name", "Cover");
            config.set("device_class", "door");
            config.set("device", v2::helper::entity_config_basic_device(device_identity_));
            config.set(v2::CoverConfig::Property::PayloadStop, nullptr);// Stop command not implemented

            return config;
        }

        v2::CoverState state() const
        {
            const auto& hatch = mgmt::device::get_device<mgmt::device::Hatch_t>(device_id_);

            switch (hatch.status()) {
                case mgmt::device::HatchState::Open:
                    return v2::CoverState::Open;
                case mgmt::device::HatchState::Closed:
                    return v2::CoverState::Closed;
                default:
                    common::logger::get(adapter::Logger)->error("Unsupported state change for cover entity with unique id: '{}'", unique_id());
                    return v2::CoverState::Closed;
            }
        }

        boost::asio::awaitable<void> async_handle_recv_value(const v2::CoverCommand& command)
        {
            if (std::holds_alternative<v2::CoverTiltCommand>(command)) {
                common::logger::get(adapter::Logger)->error("Tilt command is currently not supported");
                co_return;
            }

            const auto& cmd = std::get<v2::CoverSwitchCommand>(command);

            auto& hatch = mgmt::device::Inventory<mgmt::device::Hatch_t>.get(device_id_);
            const auto status = hatch.status();

            switch (cmd) {
                case mgmt::home_assistant::v2::CoverSwitchCommand::Open:
                    common::logger::get(adapter::Logger)->debug("HatchHandler::{}, current: {}, requested: 'open'.", __FUNCTION__, to_string(status));
                    hatch.open();
                    break;
                case mgmt::home_assistant::v2::CoverSwitchCommand::Close:
                    common::logger::get(adapter::Logger)->debug("HatchHandler::{}, current: {}, requested: 'close'.", __FUNCTION__, to_string(status));
                    hatch.close();
                    break;
                case mgmt::home_assistant::v2::CoverSwitchCommand::Stop:
                    common::logger::get(adapter::Logger)->warn("HatchHandler::{}, command not supported.", __FUNCTION__);
                    break;
            }
            co_return;
        }

    private:
        Hatch(mgmt::device::DeviceId_t device_id, adapter::Cover_t cover, DeviceIdentity device_identity)
                : EntityAdapter<adapter::Cover_t, Hatch>{std::move(cover)}
                , device_id_{std::move(device_id)}
                , device_identity_{std::move(device_identity)}
        {}

    private:
        mgmt::device::DeviceId_t device_id_;
        DeviceIdentity device_identity_;
    };
}
