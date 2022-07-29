#pragma once

#include <fstream>
#include <fmt/format.h>
#include <filesystem>

namespace {
namespace fs = std::filesystem;
}

namespace common::utils::sysfs {

inline std::string get_path(const std::string& path)
{
  // if (!fs::exists(path))
  // {
  //   throw std::runtime_error(fmt::format("Sysfs directory: {} does not exist", path));
  // }

  return path;
}

inline std::string read_attr(const fs::path& path)
{
  auto ret = std::string{};
  auto fstream = std::fstream(path, std::ios::in);

  if (!fstream) {
    throw std::runtime_error{ fmt::format("Attribute does not exist. Path: {}", path.string()) };
  }

  fstream >> ret;

  return ret;
}

template<class TValue>
inline void write_attr(const fs::path& path, const TValue& val)
{
  auto fstream = std::fstream(path, std::ios::out);

  if (!fstream) {
    throw std::runtime_error{ fmt::format("Attribute does not exist. Path: {}", path.string()) };
  }

  fstream << val;
}
}// namespace common::utils::sysfs