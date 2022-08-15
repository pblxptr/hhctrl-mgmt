#pragma once

#include <common/traits/type_tag.hpp>

namespace common::traits {
template<class T>
struct is_tuple : std::false_type
{
};

template<class... Ts>
struct is_tuple<std::tuple<Ts...>> : std::true_type
{
};

template<class T>
using is_tuple_v = is_tuple<T>::value;

template<class T>
concept IsTupleLike = is_tuple<std::decay_t<T>>::value;
template<IsTupleLike Tp>
struct TupleForEachType
{
  template<class Func>
  static constexpr void invoke(Func&& func)
  {
    using Indicies_t = std::make_index_sequence<std::tuple_size_v<Tp>>;

    on_each_type_in_sequence(std::forward<Func>(func), Indicies_t{});
  }

private:
  template<class Func, size_t... Is>
  static constexpr void on_each_type_in_sequence(Func&& func, std::index_sequence<Is...>)
  {
    (on_each_type_call<Is>(std::forward<Func>(func)), ...);
  }

  template<size_t I, class Func>
  static constexpr void on_each_type_call(Func&& func)
  {
    using Type_t = std::tuple_element_t<I, Tp>;
    func(common::traits::TypeTag<Type_t>{});
  }
};
}// namespace common::traits