#pragma once

namespace hhctrl::helpers::sysfs
{
  std::string read_attr(const fs::path& path)
  {
    auto ret = std::string{};
    auto fstream = std::fstream(path, std::ios::in);

    if (!fstream)
    {
      throw std::runtime_error{"Attribute does not exist."};
    }

    fstream >> ret;

    return ret;
  }

  template<class TValue>
  void write_attr(const fs::path& path, const TValue& val)
  {
    auto fstream = std::fstream(path, std::ios::out);

    if (!fstream)
    {
      throw std::runtime_error{"Attribute does not exist."};
    }

    fstream << val;
  }
}