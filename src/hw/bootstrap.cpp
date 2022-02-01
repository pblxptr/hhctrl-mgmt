#include "bootstrap.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <zmq.hpp>

#include <hw/board_ctrl/board_ctrl_server.hpp>
#include <hw/drivers/sysfs_hatch.hpp>
#include <hw/drivers/sysfs_led.hpp>
#include <hw/services/rgb_led_service.hpp>

namespace {
  constexpr auto BoardControlServerAddress = "tcp://127.0.0.1:9595";
}

using boost::asio::use_awaitable;

namespace hw {
void bootstrap()
{
  auto handle_coroutine = [](auto eptr)
  {
    if (eptr) {
      std::rethrow_exception(eptr);
    }
  };

  //Messaging services
  auto bctx = boost::asio::io_context{};
  auto zctx = zmq::context_t{};

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
