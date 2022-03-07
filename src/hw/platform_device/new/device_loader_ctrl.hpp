#pragma once

#include <hw/platform_device/pdtree.hpp>
#include <hw/platform_device/new/probe_context.hpp>
#include <common/traits/tuple_traits.hpp>

namespace {
namespace {
boost::json::value load_from_file(const std::string& file_path) //Todo: Move to .cpp
{
  auto file = std::ifstream{file_path};

  if (file.bad()) {
    throw std::runtime_error(fmt::format("Cannot open platform device file: {}", file_path));
  }

  auto parser = boost::json::stream_parser{};
  auto ec = boost::json::error_code{};

  do {
    auto buffer = std::array<char, 4096>{};
    file.read(buffer.begin(), buffer.size());
    parser.write(buffer.data(), file.gcount(), ec);
  } while(!file.eof());

  if (ec) {
    return nullptr;
  }
  parser.finish(ec);

  if (ec) {
    return nullptr;
  }
  return parser.release();
}
}
}


namespace hw::platform_device
{
  template<class T>
  concept DriverLoaderCompatible = requires (T a)
  {
    typename T::Compatible_t;

    { T::compatible() };
    // { T::probe() } -> std::same_as<std::add_pointer_t<typename T::Compatible_t>>;
    // -> std::convertible_to<std::string>;
  };

  template<common::traits::IsTupleLike Loaders, class DevManager>
  class DeviceLoaderCtrl
  {
    using DevManager_t = DevManager;
    using Loaders_t = Loaders;

  public:
    explicit DeviceLoaderCtrl(DevManager_t& devm)
      : devm_{devm}
    {}

    void load(const std::string& pd_file_path)
    {
      auto data = load_from_file(pd_file_path);
      const auto& array = data.as_array();
      auto context = ProbeContext<Loaders_t, DevManager_t>{devm_};

      spdlog::get("hw")->info("Platform devices to load: {}", array.size());

      for (const auto& entry : array) {
        const auto& device_descriptor = entry.as_object();
        auto driver = context.probe(device_descriptor);

        if (not driver) {
          throw std::runtime_error("Cannot load device driver");
        }
      }
    }
  private:
    DevManager_t& devm_;
  };

  template<class T, class U>
  DeviceLoaderCtrl(U&) -> DeviceLoaderCtrl<T, U>;
}