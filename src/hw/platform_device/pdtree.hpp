#pragma once

#include <boost/json.hpp>

namespace hw::platform_device
{
  using PdTreeObject_t = boost::json::object;
  using PdTreeArray_t = boost::json::array;
  using PdTreeValue_t = boost::json::value;

  std::string pdtree_to_string(const PdTreeValue_t& val);
}