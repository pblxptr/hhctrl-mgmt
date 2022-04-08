#pragma once

namespace mgmt::home_assistant
{
  void setup(AsyncEventBus& bus, DeviceRegistry<BoardDevice>& registry)
  {
    bus.subscribe<BoardDeviceInitialized>([](){
      mqtt_client_factory->create();

      const auto board = registry.get_device();
      const auto board_info = co_await board->async_board_info();
    });

    bus.subscibe<BoardDeviceStateChanged>();
  }
}