#include <hw/platform_device/pd_device_loader.hpp>

#include <fmt/format.h>
#include <fstream>
#include <spdlog/spdlog.h>
#include <iostream>

namespace {
boost::json::value load_from_file(const std::string& file_path)
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

namespace hw::platform_device
{

DriverLoader* PlatformDeviceLoader::find_loader(const std::string_view compatible) const
{
  auto loader = std::find_if(loaders_.begin(), loaders_.end(), [&compatible](const auto& l)
  {
    return l->compatible() == compatible;
  });

  if (loader == loaders_.end()) {
    return nullptr;
  }
  return loader->get();
}

DriverLoader* PlatformDeviceLoader::find_loader(const hw::platform_device::PdTreeObject_t& device_descriptor) const
{
  if (not device_descriptor.contains("compatible")) {
    spdlog::get("hw")->error("Device descritor does not have compatible attribute");
    return nullptr;
  }
  const auto compatible = device_descriptor.at("compatible").as_string().c_str();

  return find_loader(compatible);
}

void PlatformDeviceLoader::load(const std::string& pd_file_path)
{
  auto data = load_from_file(pd_file_path);
  const auto& array = data.as_array();

  spdlog::get("hw")->info("Platform devices to load: {}", array.size());

  for (const auto& entry : array)
  {
    const auto& device_descriptor = entry.as_object();
    auto loader = find_loader(device_descriptor);

    if (!loader) {
      throw std::runtime_error("Cannot load device driver, loader not found");
    }

    loader->probe(device_descriptor);

    // if (not device_driver) {
      // throw std::runtime_error("Cannot load device driver");
    // }
    spdlog::get("hw")->info("Device driver has been loaded");
  }
}
}