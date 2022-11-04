#pragma once

#include <main_board/platform/device_loader.hpp>
#include <main_board/platform/pdtree.hpp>
#include <main_board/device/sysfs_cpu_temp_sensor.hpp>
#include <device/device_register.hpp>
#include <poller/polling_service.hpp>
#include <poller/temp_sensor_poller.hpp>
#include <poller/poller_factory.hpp>

namespace mgmt::platform_device {
class SysfsCPUTempSensorProvider
{
public:
  SysfsCPUTempSensorProvider(
    mgmt::device::PollingService& polling_service,
    mgmt::poller::PollerFactory& poller_factory)
    : polling_service_{ polling_service }
    , poller_factory_{ poller_factory }
  {}
  static constexpr auto compatible()
  {
    return "sysfs_cpu_temp_sensor";
  }

  template<class BoardBuilder>
  bool load(BoardBuilder& builder, const PdTreeObject_t& object)
  {
    constexpr auto sysfs_path_atrr = "sysfs_path";
    constexpr auto model_attr = "model";

    common::logger::get(mgmt::device::Logger)->debug("SysfsCPUTempSensorProvider: probe driver '{}'", pdtree_to_string(object.at(model_attr)));

    if (not object.contains("sysfs_path")) {
      common::logger::get(mgmt::device::Logger)->error("Missing attribute 'sysfs_path' id pdtree for sysfs_cpu_temp_sensor driver descriptor");
      return false;
    }

    auto sysfs_path = pdtree_get<std::string>(object, sysfs_path_atrr);

    builder.template add_loader<mgmt::device::TempSensor_t>(DeviceLoader{
      .load = [this, sysfs_path = std::move(sysfs_path)]() {
          auto device_id = mgmt::device::register_device<mgmt::device::TempSensor_t>(
            mgmt::device::SysfsCPUTempSensor{ sysfs_path }
          );
          polling_service_.add_poller(
            device_id,
            std::chrono::seconds(2),
            poller_factory_.create_poller<mgmt::poller::TempSensorPoller>(device_id)
          );
          return device_id; },
      .unload = [](int) { throw std::runtime_error("Not supported"); } });

    return true;
  }

private:
  mgmt::device::PollingService& polling_service_;
  mgmt::poller::PollerFactory& poller_factory_;
};
}// namespace mgmt::platform_device
