#pragma once

namespace common::traits {
template<class T>
struct TypeTag
{
  using Type_t = T;
};
}// namespace common::traits