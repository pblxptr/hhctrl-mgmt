#pragma once

#include <tuple>

namespace common::utils {
template<class... Args>
auto capture_fwd(Args&&... args)
{
  return std::tuple<Args...>(std::forward<Args...>(args)...);
}
}// namespace common::utils