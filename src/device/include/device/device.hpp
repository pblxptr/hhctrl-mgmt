#pragma once

#include <type_traits>

namespace mgmt::device {
template<class T>
concept Device =
  std::is_object_v<T> and(not std::is_pointer_v<T>)
  and ((not std::is_const_v<T>)and(not std::is_volatile_v<T>));
}// namespace mgmt::device
