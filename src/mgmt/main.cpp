// #include <iostream>
// #include <thread>
// #include <fstream>
// #include <filesystem>
// #include <spdlog/spdlog.h>

// #include <common/utils/datetime.hpp>
// #include <common/scheduler/scheduler.hpp>
// #include <common/scheduler/file_task_store.hpp>
// #include <hw/drivers/sysfs_hatch.hpp>
// #include <hw/drivers/sysfs_led.hpp>
// #include <hw/services/rgb_led_service.hpp>
// #include <eci_hc.pb.h>
// #include <bci.pb.h>
// #include <atomic>
// #include <icon/endpoint/endpoint_config.hpp>
// #include <icon/endpoint/endpoint.hpp>
// #include <zmq.hpp>

// namespace scheduler = common::scheduler;
// namespace fs = std::filesystem;

// int main()
// {
//   using namespace date;

//   spdlog::set_level(spdlog::level::debug);

//   auto bctx = boost::asio::io_context{};
//   auto zctx = zmq::context_t{};
//   auto endpoint = icon::setup_default_endpoint(
//     icon::use_services(bctx, zctx),
//     icon::address("tcp://127.0.0.1:5555"),
//     icon::consumer<bci::GetVisualIndicationReq>(
//       [](icon::MessageContext<bci::GetVisualIndicationReq> context) -> awaitable<void> {
//         co_return;
//       }))
//     .build();


//   // auto io = boost::asio::io_context{};
//   // auto work = boost::asio::io_context::work{io};

//   // auto hatch = hw::drivers::SysfsHatch{"/sys/class/hatch2sr/hatch2sr"};
//   // auto red = hw::drivers::SysfsLed{"/sys/class/leds/red"};
//   // auto green = hw::drivers::SysfsLed{"/sys/class/leds/green"};
//   // auto blue = hw::drivers::SysfsLed{"/sys/class/leds/blue"};
//   // auto led_service = hw::services::RgbLedService{red, green, blue};

//   // auto store = scheduler::FileTaskStore("./tasks.json");
//   // auto s = scheduler::Scheduler{io, store};



//   // s.every(std::chrono::days(1), [](){ spdlog::info("handler: std::chrono::days(1)"); });
//   // s.every(std::chrono::days(2), [](){ spdlog::info("handler: std::chrono::days(2)"); });
//   // s.every(std::chrono::days(3), [](){ spdlog::info("handler: std::chrono::days(3)"); });

//   // io.run();

//   return 0;
// }