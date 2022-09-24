#pragma once

#include <string_view>

namespace common::utils {
namespace impl {
  template<class Value>
  consteval auto map_value(Value val) { return val; }

  template<class CharT>
  consteval std::basic_string_view<CharT> map_value(const CharT* val) { return { val }; }

  template<class Value>
  using MapValueType_t = decltype(map_value(std::declval<Value>()));

  template<class Key, class Value>
  struct BaseMapper
  {
    using Key_t = MapValueType_t<Key>;
    using Value_t = MapValueType_t<Value>;
    using KeyValuePair_t = std::pair<Key_t, Value_t>;

    const KeyValuePair_t Kvp;

    consteval BaseMapper(std::pair<Key_t, Value_t> kvp) //NOLINT(modernize-pass-by-value)
      : Kvp{ kvp }
    {}
  };
}// namespace impl

template<class KV, class... Rest>
class Mapper : public impl::BaseMapper<typename KV::first_type, typename KV::second_type>
{
  using Base_t = impl::BaseMapper<
    typename KV::first_type,
    typename KV::second_type>;

  const Mapper<Rest...> Next;

public:
  consteval Mapper(typename Base_t::KeyValuePair_t kvp, Rest... rest)
    : Base_t{ kvp }
    , Next{ rest... }
  {}

  // //Map by key
  constexpr auto map(const typename Base_t::Key_t& key) const
  {
    const auto& [k, v] = Base_t::Kvp;

    return key == k ? v : Next.map(key);
  }

  // //Map by value - reverse
  constexpr auto map(const typename Base_t::Value_t& value) const
  {
    const auto& [k, v] = Base_t::Kvp;

    return value == v ? k : Next.map(value);
  }
};

template<
  class Key,
  class Value>
class Mapper<std::pair<Key, Value>> : public impl::BaseMapper<Key, Value>
{
  using Base_t = impl::BaseMapper<Key, Value>;

public:
  consteval Mapper(typename Base_t::KeyValuePair_t kvp)
    : Base_t{ kvp }
  {}

  constexpr auto map(const typename Base_t::Key_t& key) const
  {
    const auto& [k, v] = Base_t::Kvp;

    return automap(key, k, v);
  }

  constexpr auto map(const typename Base_t::Value_t& value) const
  {
    const auto& [k, v] = Base_t::Kvp;

    return automap(value, v, k);
  }

private:
  template<
    class Provided,
    class Actual,
    class Ret>
  constexpr auto automap(const Provided& provided, const Actual& actual, const Ret& ret) const
  {
    if (provided != actual) {
      throw std::invalid_argument("Key does not exist");
    }

    return ret;
  }
};

template<class Key, class Value, class... Args>
Mapper(std::pair<Key, Value>, Args...) -> Mapper<std::pair<impl::MapValueType_t<Key>, impl::MapValueType_t<Value>>, Args...>;
}// namespace common::utils
