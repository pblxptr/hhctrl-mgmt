#pragma once

#include <boost/json.hpp>
#include <fmt/format.h>
#include <sstream>

namespace mgmt::platform_device
{
  using PdTreeObject_t = boost::json::object;
  using PdTreeArray_t = boost::json::array;
  using PdTreeValue_t = boost::json::value;

  boost::json::value pdtree_load(const std::string& file_path);

  std::string pdtree_to_string(const PdTreeValue_t& val);
  template<class T, class Key>
  T pdtree_get(const PdTreeObject_t& object, const Key& key)
  {
    if (not object.contains(key)) {
      throw std::runtime_error(fmt::format("Property '{}' does not exist in pdtree", key));
    }

    const auto& value = object.at(key);
    auto iss = std::istringstream{value.as_string().c_str()};

    auto ret = T{};

    iss >> ret;

    if (iss.fail() || iss.bad()) {
      throw std::runtime_error(fmt::format("Cannot read property '{}' from pdtree", key));
    }

    return ret;
  }
}