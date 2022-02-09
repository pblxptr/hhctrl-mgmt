#include "bootstrap.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <zmq.hpp>

#include <hw/board_ctrl/board_ctrl_server.hpp>
#include <hw/drivers/sysfs_hatch.hpp>
#include <hw/drivers/sysfs_led.hpp>
#include <hw/services/rgb_led_service.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <icon/utils/logger.hpp>

namespace {
  constexpr auto BoardControlServerAddress = "tcp://127.0.0.1:9595";
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

  //Hw services
  auto hatch = hw::drivers::SysfsHatch{"/sys/class/hatch2sr/hatch2sr"};
  auto red = hw::drivers::SysfsLed{"/sys/class/leds/red"};
  auto green = hw::drivers::SysfsLed{"/sys/class/leds/green"};
  auto blue = hw::drivers::SysfsLed{"/sys/class/leds/blue"};
  auto led_service = hw::services::RgbLedService{red, green, blue};

  //Serves
  auto bci_server = hw::board_ctrl::BoardControlServer{
    led_service,
    bctx,
    zctx,
    BoardControlServerAddress
  };
  boost::asio::co_spawn(bctx, bci_server.run(), handle_coroutine);

  bctx.run();
}
}
