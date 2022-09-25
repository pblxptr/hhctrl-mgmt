#pragma once

namespace common::utils {
template<class... Ts>
struct Overloaded : Ts...
{
  using Ts::operator()...;
};
}// namespace common::utils
