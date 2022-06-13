
#include <cassert>

#include <filesystem>
#include <vector>
#include <variant>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <boost/asio/executor_work_guard.hpp>

#include <common/event/event_bus.hpp>

#include <static/devicetree.hpp>
#include <static/device_register.hpp>
#include <static/main_board.hpp>
#include <static/sysfs_led.hpp>
#include <static/sysfs_hatch.hpp>

#include <static/events/device_created.hpp>
#include <static/events/device_removed.hpp>
#include <static/pdtree.hpp>
#include <static/device_id.hpp>
#include <static/devicetree.hpp>

using WorkGuard_t =
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;

namespace mgmt::device {
class SysfsHatchLoader
{
public:
  SysfsHatchLoader(mgmt::device::DeviceTree& dtree, common::event::AsyncEventBus& bus)
    : dtree_{ dtree }
    , bus_{ bus }
  {}

  bool load(const mgmt::device::DeviceId_t& parent_dev_id) const
  {
    const auto id = mgmt::device::register_device<mgmt::device::SysfsHatch>("sysfs_path");

    auto on_parent_removed = [id, this]() {
      mgmt::device::deregister_device<mgmt::device::SysfsHatch>(id);
      // bus_.publish(mgmt::event::DeviceRemoved<SysfsHatch>{id});
    };

    dtree_.add_child(parent_dev_id, id, std::move(on_parent_removed));
    // bus_.publish(mgmt::event::DeviceCreated<mgmt::device::SysfsHatch>{id});

    return true;
  }

  constexpr auto compatible() const
  {
    return "sysfs_hatch";
  }
private:
  common::event::AsyncEventBus& bus_;
  mgmt::device::DeviceTree& dtree_;
};
}

namespace mgmt::device {
class SysfsLedLoader
{
public:
  SysfsLedLoader(mgmt::device::DeviceTree& dtree, common::event::AsyncEventBus& bus)
    : dtree_{ dtree }
    , bus_{ bus }
  {}

  bool load(const mgmt::device::DeviceId_t&) const
  {
    const auto id = mgmt::device::register_device<mgmt::device::SysfsLed>("sysfs_path");

    // bus_.publish(mgmt::event::DeviceCreated<mgmt::device::SysfsLedLoader>{
    //   .device_id = id
    // });

    return true;
  }

  constexpr auto compatible() const
  {
    return "sysfs_led";
  }
private:
  common::event::AsyncEventBus& bus_;
  mgmt::device::DeviceTree& dtree_;
};
}

using PlatformDeviceLoader_t = std::variant<
  mgmt::device::SysfsHatchLoader,
  mgmt::device::SysfsLedLoader
>;

template<class T>
concept Loader = requires(T v)
{
  { v.compatible() } -> std::same_as<const char*>;
  { v.load(std::declval<const mgmt::device::DeviceId_t&>()) } -> std::same_as<bool>;
};

class PlatformDeviceLoader
{
public:
  PlatformDeviceLoader(
    std::string pdtree_file_path,
    std::vector<PlatformDeviceLoader_t> loaders
  )
    : pdtree_file_path_ { std::move(pdtree_file_path) }
    , loaders_ { std::move(loaders) }
  {}

  bool load_devices(const mgmt::device::DeviceId_t& parent_id) const
  {
    const auto compatibles = std::vector<std::string> {
      "sysfs_hatch",
      "sysfs_led"
    };

    for (const auto& compatible : compatibles) {
      auto loader = std::ranges::find_if(loaders_, [&compatible](auto&& l) {
        return std::visit([](Loader auto&& l) { return l.compatible(); }, l) == compatible;
      });

      if (loader == loaders_.end()) {
        fmt::print("Cannot find loader compatible with: {}\n", compatible);

        return false;
      }

      const auto loaded = std::visit([parent_id](Loader auto& l) { return l.load(parent_id); }, *loader);
      if (not loaded) {
        fmt::print("Device cannot be loaded\n");
      }
    }
    return true;
  }
private:
  std::string pdtree_file_path_;
  std::vector<PlatformDeviceLoader_t> loaders_;
};

void board_init(common::event::AsyncEventBus& bus, const PlatformDeviceLoader& platform_dev_loader)
{
  const auto id = mgmt::device::register_device<mgmt::device::MainBoard>();
  
  bus.publish(mgmt::event::DeviceCreated<mgmt::device::MainBoard>{id}); 

  platform_dev_loader.load_devices(id);
}

int main()
{
  static auto mgmt_logger = spdlog::stdout_color_mt("mgmt");
  mgmt_logger->set_level(spdlog::level::debug);
  mgmt_logger->info("Booststrap: mgmt");

  // //Messaging services
  auto bctx = boost::asio::io_context{};
  // auto zctx = zmq::context_t{};
  auto work_guard = WorkGuard_t{bctx.get_executor()};
  // auto command_dispatcher = common::command::AsyncCommandDispatcher{};
  auto event_bus = common::event::AsyncEventBus{bctx};
  auto dtree = mgmt::device::DeviceTree{};

  event_bus.subscribe<mgmt::event::DeviceCreated<mgmt::device::MainBoard>>(
    [](auto&& event) -> boost::asio::awaitable<void> {
      spdlog::get("mgmt")->debug("MainBoard device created, device id: {}", event.device_id);
      co_return;
    }
  );

  event_bus.subscribe<mgmt::event::DeviceCreated<mgmt::device::SysfsHatch>>(
    [](auto&& event) -> boost::asio::awaitable<void> {
      spdlog::get("mgmt")->debug("SysfsHatch device created, device id: {}", event.device_id);
      co_return;
    }
  );

  board_init(event_bus, PlatformDeviceLoader{
    "pdtree.json",
    std::vector<PlatformDeviceLoader_t>{
      mgmt::device::SysfsLedLoader{dtree, event_bus},
      mgmt::device::SysfsHatchLoader{dtree, event_bus}
    }
  });

  bctx.run();
}