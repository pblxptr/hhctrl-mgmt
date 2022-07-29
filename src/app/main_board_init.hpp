#pragma once

#include "device/polling_service.hpp"
#include <boost/functional/hash.hpp>
#include <common/event/event_bus.hpp>
#include <device/devicetree.hpp>
#include <device/hardware_identity_store.hpp>
#include <device/logger.hpp>
#include <events/device_state_changed.hpp>
#include <main_board/platform/generic_device_loading_strategy.hpp>
#include <main_board/platform/hatch2sr_provider.hpp>
#include <main_board/platform/platform_builder.hpp>
#include <main_board/platform/platform_device_discovery.hpp>
#include <main_board/platform/sysfs_rgb_indicator_provider.hpp>
#include <main_board/platform/sysfs_temp_sensor_provider.hpp>
#include <poller/poller_factory.hpp>
#include <poller/main_board_poller.hpp>

namespace mgmt::app
{
  void main_board_init(
    std::string pdtree_path,
    mgmt::device::DeviceTree& dtree,
    mgmt::device::HardwareIdentityStore_t& hw_identity_store,
    mgmt::device::PollingService& polling_service,
    common::event::AsyncEventBus& bus
  )
  {
    using namespace mgmt::platform_device;

    //Prepare
    auto poller_factory = mgmt::poller::PollerFactory{bus};
    auto builder = PlatformBuilder<DefaultGenericDeviceLoadingStrategy>{};
    auto platform_device_discovery = PlatformDeviceDiscovery{
      pdtree_path,
        RGBIndicatorProvider{},
        HatchProvider{polling_service, poller_factory}
    };
    platform_device_discovery.setup(builder);

    //Handle main board
    auto board_id = mgmt::device::register_device(std::move(builder).build_board());
    const auto& board = mgmt::device::get_device<mgmt::device::MainBoard>(board_id);
    hw_identity_store.emplace(board_id, board.hardware_identity());
    polling_service.add_poller(board_id,
      std::chrono::seconds(5),
      poller_factory.create_poller<mgmt::poller::MainBoardPoller>(board_id)
    );
    bus.publish(mgmt::event::DeviceCreated<mgmt::device::MainBoard> {
      board_id
    });

    //Handle generic devices connected to board
    auto generic_dev_loader_handler = DefaultGenericDeviceLoadingStrategy{
      board_id,
      dtree,
      bus
    };
    auto generic_dev_loaders = std::move(builder).build_generic_loaders();
    for (auto&& loader : generic_dev_loaders) {
      loader(generic_dev_loader_handler);
    }
  }
}