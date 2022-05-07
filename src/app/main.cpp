
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <newnew/main_board.hpp>
#include <newnew/sysfs_hatch.hpp>
#include <newnew/sysfs_led.hpp>
#include <newnew/rgb3_led.hpp>
#include <newnew/platform_device_scanner.hpp>
#include <newnew/platform_device_loader.hpp>
#include <newnew/device_pooling_manager.hpp>
#include <newnew/device_ref_registry.hpp>
#include <newnew/disposable_device.hpp>

// #include <common/utils/disposable.hpp>

// class HatchCreator
// {
// public:
//   HatchCreator(mgmt::pooler::DevicePoolingManager& pooling_manager)
//     : pooling_manager_{pooling_manager}
//   {}

//   template<class T, class...Args>
//   void create(Args&&... args)
//   {
//     auto device = common::utils::make_disposable<>();
//   }
// private:
//   mgmt::pooler::DevicePoolingManager pooling_manager_;
// };

// class PlatformDeviceHatchLoader : public mgmt::platform_device::DeviceLoader
// {
// public:
//   constexpr static inline auto SysfsPathAttr { "sysfs_path" };

//   bool is_compatible(const std::string& compatible) const
//   {
//     return compatible == "sysfs_hatch";
//   }
//   void load(mgmt::device::Device& parent, const mgmt::platform_device::PdTreeObject_t& descriptor) const
//   {
//     if (not descriptor.contains(SysfsPathAttr)) {
//       throw std::runtime_error("Missing attribute 'sysfs_path' id pdtree for hatch2sr driver descriptor");
//     }

//     auto dev = creator_.create<mgmt::device::SysfsHatch>(
//       mgmt::device::DeviceId::new_id(),
//       pdtree_get<std::string>(descriptor->as_object(), SysfsPathAttr)
//     );

//     parent.add_device(std::move(dev));
//   }
// };


int main()
{
  static auto mgmt_logger = spdlog::stdout_color_mt("mgmt");
  mgmt_logger->set_level(spdlog::level::debug);
  mgmt_logger->info("Booststrap: mgmt");

  // //Messaging services
  // auto bctx = boost::asio::io_context{};
  // auto zctx = zmq::context_t{};
  // auto work_guard = work_guard_type{bctx.get_executor()};
  // auto command_dispatcher = common::command::AsyncCommandDispatcher{};
  // auto event_bus = common::event::AsyncEventBus{bctx};


  auto devreg = mgmt::device::DeviceRefRegistry{};
  auto obj_deleter = [&devreg](const auto& dev) {
    if (devreg.exists(dev.id()))
      devreg.remove(dev.id());
  };

  //Main board
  auto main_board = mgmt::device::make_disposable_device<mgmt::device::MainBoard>(
    obj_deleter,
    mgmt::device::DeviceId::new_id()
  );
  devreg.add(*main_board);

  //Hatch
  auto hatch = mgmt::device::make_disposable_device<mgmt::device::SysfsHatch>(obj_deleter,
    mgmt::device::DeviceId::new_id(),
    "sysfs_hatch"
  );
  devreg.add(*hatch);

  main_board->add_device(std::move(hatch));

  }