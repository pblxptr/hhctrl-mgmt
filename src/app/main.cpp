#include <filesystem>
#include <cassert>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <poller/polling_service.hpp>

#include <app/logger.hpp>
#include <app/main_board_init.hpp>
#include <app/app_config.hpp>
#include <app/home_assistant.hpp>

#include <cstdlib>
#include <iostream>

using WorkGuard_t =
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;

int main(int argc, char** argv)
{
  try {
    if (argc != 2) {
      common::logger::get(mgmt::app::Logger)->error("Too few arguments");
      std::cerr << "Too few arguments.\n";
      std::cerr << fmt::format("Example: {} /path/to/config.json\n", argv[0]);

      return EXIT_FAILURE;
    }

    /* General Services */
    auto bctx = boost::asio::io_context{};
    auto work_guard = WorkGuard_t{ bctx.get_executor() };
    auto hw_identity_store = mgmt::device::HardwareIdentityStore_t{};
    auto dtree = mgmt::device::DeviceTree{};
    auto bus = common::event::AsyncEventBus{ bctx };
    auto config = mgmt::app::load_config(argv[1]);

    /* Logger */
    mgmt::app::logger_init(config.log_dir);

    /* Home Assistant Services */
    mgmt::app::home_assistant_init(mgmt::app::HomeAssistantServices {
      .context = bctx,
      .dtree = dtree,
      .hw_identity_store = hw_identity_store,
      .bus = bus,
      .config = config
    });

    /* Device Services */
    auto polling_service = mgmt::device::PollingService{ std::ref(bctx) };

    spdlog::get("mgmt")->info("App config: {}", pretty_format_config(config));

    /* Run */
    boost::asio::co_spawn(
      bctx, [pdtree_path = config.dtree_file, &dtree, &hw_identity_store, &bus, &polling_service]() -> boost::asio::awaitable<void> {
        mgmt::app::main_board_init(
          pdtree_path,
          dtree,
          hw_identity_store,
          polling_service,
          bus);

        co_return;
      },
      common::coro::rethrow);
    bctx.run();
  } catch (const std::exception& exception) {
    common::logger::get(mgmt::app::Logger)->error(fmt::format("Exception has been thrown: {}", exception.what()));
    std::abort();
  }
}
