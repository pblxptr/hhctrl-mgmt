#pragma once

namespace common::traits {
  template<class T>
  struct is_tuple : std::false_type{};

  template<class...Ts>
  struct is_tuple<std::tuple<Ts...>> : std::true_type{};

  template<class T>
  using is_tuple_v = is_tuple<T>::value;

  template<class T>
  concept IsTupleLike = is_tuple<std::decay_t<T>>::value;
}