#include "bootstrap.hpp"

#include <fstream>
#include <boost/asio/io_context.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <zmq.hpp>
#include <icon/utils/logger.hpp>

#include <hw/board_ctrl/board_ctrl_server.hpp>
// #include <hw/platform_device/pd_device_loader.hpp>

#include <hw/platform_device/device_manager.hpp>
#include <hw/platform_device/device_loader_ctrl.hpp>
#include <hw/platform_device/loaders/hatch2sr_driver_loader.hpp>
#include <hw/platform_device/loaders/sysfsled_driver_loader.hpp>
#include <hw/platform_device/loaders/rgb3led_driver_loader.hpp>
#include <hw/platform_device_ctrl/pdctrl_server.hpp>


void create_pdtree_for_tests()
{
const char* json = R"(
[
  {
    "name" : "rgb_led",
    "compatible" : "rgb3led",
    "leds" : [
      {
        "name"      : "led",
        "compatible" : "sysfs_led",
        "sysfs_path" : "/tmp/leds/red",
        "color" : "red"
      },
      {
        "name"      : "led",
        "compatible" : "sysfs_led",
        "sysfs_path" : "/tmp/leds/green",
        "color" : "green"
      },
      {
        "name"      : "led",
        "compatible" : "sysfs_led",
        "sysfs_path" : "/tmp/leds/blue",
        "color" : "blue"
      }
    ]
  },
  {
    "name" : "hatch2sr",
    "compatible" : "sysfs_hatch2sr",
    "sysfs_path" : "/tmp/misc/hatch2sr"
  }
]
)";

  auto file = std::ofstream{"/tmp/pdtree.json"};

  file << json << '\n';
}


namespace {
  constexpr auto BoardControlServerAddress = "tcp://127.0.0.1:9595";
  constexpr auto PlatformDeviceControlServerAddress = "tcp://127.0.0.1:9596";
}

using boost::asio::use_awaitable;
using work_guard_type =
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;

namespace hw {
void bootstrap()
{
  auto icon_logger = icon::utils::setup_logger();
  auto hw_logger = spdlog::stdout_color_mt("hw");
  
  spdlog::set_level(spdlog::level::debug);

  hw_logger->info("Booststrap: hw");

  auto handle_coroutine = [](auto eptr)
  {
    if (eptr) {
      std::rethrow_exception(eptr);
    }
  };

  //Messaging services
  auto bctx = boost::asio::io_context{};
  auto zctx = zmq::context_t{};
  work_guard_type work_guard(bctx.get_executor());


  //new
  create_pdtree_for_tests();
  using SupportedDeviceInterfaces_t = std::tuple<
    hw::drivers::LedDriver,
    hw::drivers::RGBLedDriver,
    hw::drivers::HatchDriver
  >;

  using SupportedDeviceLoaders_t = std::tuple<
    hw::platform_device::SysfsLedDriverLoader,
    hw::platform_device::RGBLedDriverLoader,
    hw::platform_device::Hatch2srDriverLoader
  >;

  auto devm = hw::platform_device::DeviceManager<SupportedDeviceInterfaces_t>{};
  auto pd_loader_ctrl = hw::platform_device::DeviceLoaderCtrl<SupportedDeviceLoaders_t, decltype(devm)>{devm};
  pd_loader_ctrl.load("/tmp/pdtree.json");

  hw::pdctrl::PlatformDeviceCtrlServer pdctrl_server = hw::pdctrl::PlatformDeviceCtrlServer{
      bctx, zctx, devm, PlatformDeviceControlServerAddress
  };

  boost::asio::co_spawn(bctx, pdctrl_server.run(), handle_coroutine);

  // //Hw services

  // auto hatch = hw::drivers::SysfsHatchDriver{"/sys/class/hatch2sr/hatch2sr"};
  // auto red = hw::drivers::SysfsLedDriver{"/sys/class/leds/red"};
  // auto green = hw::drivers::SysfsLedDriver{"/sys/class/leds/green"};
  // auto blue = hw::drivers::SysfsLedDriver{"/sys/class/leds/blue"};
  // auto led_service = hw::services::RgbLedService{red, green, blue};

  // //Serves
  // auto bci_server = hw::board_ctrl::BoardControlServer{
  //   led_service,
  //   bctx,
  //   zctx,
  //   BoardControlServerAddress
  // };
  // boost::asio::co_spawn(bctx, bci_server.run(), handle_coroutine);

  bctx.run();
}
}

