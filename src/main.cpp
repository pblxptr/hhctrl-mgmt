#include <mgmt/bootstrap.hpp>
#include <hw/bootstrap.hpp>
#include <thread>
#include <spdlog/spdlog.h>
int main()
{
  //Each subsystem is working in a separate thread. TOOD: Move them to separate processes.
  
  spdlog::set_level(spdlog::level::debug);
  auto mgmt_thread = std::thread(mgmt::bootstrap);
  auto hw_thread = std::thread(hw::bootstrap);

  mgmt_thread.join();
  hw_thread.join();
}
