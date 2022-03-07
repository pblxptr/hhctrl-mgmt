#include <hw/platform_device/pdtree.hpp>

namespace hw::platform_device
{
  std::string pdtree_to_string(const PdTreeValue_t& val)
  {
    return val.as_string().c_str();
  }
}