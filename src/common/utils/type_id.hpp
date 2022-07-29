#pragma once

#include <cstddef>
#include <atomic>

namespace common::utils {
class TypeId
{
public:
  template<class T>
  static size_t value()
  {
    using Type_t = std::decay_t<T>;

    return get_id<Type_t>();
  }

private:
  template<class Tp>
  static size_t get_id()
  {
    static auto id = ids_++;

    return id;
  }
  static inline std::atomic<size_t> ids_{};
};
}// namespace common::utils