#pragma once

#include <main_board/platform/platform_device_discovery.hpp>
#include <main_board/platform/platform_builder.hpp>
#include <main_board/platform/generic_device_loading_strategy.hpp>
#include <main_board/platform/hatch2sr_provider.hpp>
#include <main_board/platform/sysfs_rgb_indicator_provider.hpp>
#include <main_board/platform/sysfs_temp_sensor_provider.hpp>
#include <common/event/event_bus.hpp>
#include <inventory/devicetree.hpp>

namespace mgmt::app
{
  void main_board_init(
    std::string pdtree_path,
    mgmt::device::DeviceTree& dtree,
    common::event::AsyncEventBus& bus
  )
  {
    using namespace mgmt::platform_device;

    //Prepare
    auto builder = PlatformBuilder<DefaultGenericDeviceLoadingStrategy>{};
    auto platform_device_discovery = PlatformDeviceDiscovery{
      pdtree_path,
        RGBIndicatorProvider{},
        HatchProvider{}
    };
    platform_device_discovery.setup(builder);

    //Handle main board
    auto board = std::move(builder).build_board();
    auto board_id = mgmt::device::register_device(std::move(board));
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