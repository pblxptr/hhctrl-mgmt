#pragma once

#include <fstream>
#include <fmt/format.h>

namespace {
  namespace fs = std::filesystem;
}

namespace hhctrl::helpers::sysfs
{
  inline std::string read_attr(const fs::path& path)
  {
    auto ret = std::string{};
    auto fstream = std::fstream(path, std::ios::in);

    if (!fstream) {
      throw std::runtime_error{fmt::format("Attribute does not exist. Path: {}", path.string())};
    }

    fstream >> ret;

    return ret;
  }

  template<class TValue>
  inline void write_attr(const fs::path& path, const TValue& val)
  {
    auto fstream = std::fstream(path, std::ios::out);

    if (!fstream) {
      throw std::runtime_error{fmt::format("Attribute does not exist. Path: {}", path.string())};
    }

    fstream << val;
  }
}