#include <home_assistant/device/hatch.hpp>
#include <device/hatch_t.hpp>

namespace mgmt::home_assistant::device
{
    Hatch::Hatch(mgmt::device::DeviceId_t device_id, v2::DeviceIdentity device_identity, adapter::Cover_t cover)
        : EntityAdapter<adapter::Cover_t, Hatch>{std::move(cover)}
        , device_id_{std::move(device_id)}
        , device_identity_{std::move(device_identity)}
    {}

    boost::asio::awaitable<std::optional<Hatch>> Hatch::async_create(
            mgmt::device::DeviceId_t device_id,
            const adapter::DeviceIdentityProvider& identity_provider,
            const adapter::EntityFactory& factory
    )
    {
        auto cover = factory.create_cover(adapter::get_unique_id(device_id, identity_provider.identity(device_id)));
        auto self = Hatch{device_id, identity_provider.identity(device_id), std::move(cover)};

        const auto initialized = co_await self.async_init();
        if (!initialized) {
            co_return std::nullopt;
        }

        co_return self;
    }

    const mgmt::device::DeviceId_t& Hatch::hardware_id() const
    {
        return device_id_;
    }

    v2::EntityConfig Hatch::config() const
    {
        auto config = v2::EntityConfig{};
        config.set("name", "Cover");
        config.set("device_class", "door");
        config.set("device", v2::helper::entity_config_basic_device(device_identity_));
        config.set(v2::CoverConfig::Property::PayloadStop, nullptr);// Stop command not implemented

        return config;
    }


    v2::CoverState Hatch::state() const
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

    boost::asio::awaitable<void> Hatch::async_handle_recv_value(const v2::CoverCommand& command)
    {
        common::logger::get(adapter::Logger)->trace("Hatch::{}", __FUNCTION__);

        if (std::holds_alternative<v2::CoverTiltCommand>(command)) {
            common::logger::get(adapter::Logger)->error("Tilt command is currently not supported");
            co_return;
        }

        const auto& cmd = std::get<v2::CoverSwitchCommand>(command);

        auto& hatch = mgmt::device::Inventory<mgmt::device::Hatch_t>.get(device_id_);
        const auto status = hatch.status();

        switch (cmd) {
            case mgmt::home_assistant::v2::CoverSwitchCommand::Open:
                common::logger::get(adapter::Logger)->debug("Hatch::{}, current: {}, requested: 'open'.", __FUNCTION__, to_string(status));
                hatch.open();
                break;
            case mgmt::home_assistant::v2::CoverSwitchCommand::Close:
                common::logger::get(adapter::Logger)->debug("Hatch::{}, current: {}, requested: 'close'.", __FUNCTION__, to_string(status));
                hatch.close();
                break;
            case mgmt::home_assistant::v2::CoverSwitchCommand::Stop:
                common::logger::get(adapter::Logger)->warn("Hatch::{}, command not supported.", __FUNCTION__);
                break;
        }
        co_return;
    }
}
