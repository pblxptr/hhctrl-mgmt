#pragma once

#include <string_view>

namespace common::utils {
namespace impl {
  template<class Value>
  consteval auto map_value(Value v) { return v; }

  template<class CharT>
  consteval std::basic_string_view<CharT> map_value(const CharT* v) { return { v }; }

  template<class Value>
  using MapValueType_t = decltype(map_value(std::declval<Value>()));

  template<class Key, class Value>
  struct BaseMapper
  {
    using Key_t = MapValueType_t<Key>;
    using Value_t = MapValueType_t<Value>;
    using KeyValuePair_t = std::pair<Key_t, Value_t>;

    const KeyValuePair_t m_kv;

    consteval BaseMapper(std::pair<Key_t, Value_t> kv)
      : m_kv{ kv }
    {}
  };
}// namespace impl

template<class KV, class... Rest>
class Mapper : public impl::BaseMapper<typename KV::first_type, typename KV::second_type>
{
  using Base_t = impl::BaseMapper<
    typename KV::first_type,
    typename KV::second_type>;

  const Mapper<Rest...> m_next;

public:
  consteval Mapper(Base_t::KeyValuePair_t kv, Rest... rest)
    : Base_t{ kv }
    , m_next{ rest... }
  {}

  // //Map by key
  constexpr auto map(const Base_t::Key_t& key) const
  {
    const auto& [k, v] = Base_t::m_kv;

    return key == k ? v : m_next.map(key);
  }

  // //Map by value - reverse
  constexpr auto map(const Base_t::Value_t& value) const
  {
    const auto& [k, v] = Base_t::m_kv;

    return value == v ? k : m_next.map(value);
  }
};

template<
  class Key,
  class Value>
class Mapper<std::pair<Key, Value>> : public impl::BaseMapper<Key, Value>
{
  using Base_t = impl::BaseMapper<Key, Value>;

public:
  consteval Mapper(Base_t::KeyValuePair_t kv)
    : Base_t{ kv }
  {}

  constexpr auto map(const Base_t::Key_t& key) const
  {
    const auto& [k, v] = Base_t::m_kv;

    return automap(key, k, v);
  }

  constexpr auto map(const Base_t::Value_t& value) const
  {
    const auto& [k, v] = Base_t::m_kv;

    return automap(value, v, k);
  }

private:
  template<
    class Provided,
    class Actual,
    class Ret>
  constexpr auto automap(const Provided& provided, const Actual& actual, const Ret& ret) const
  {
    if (provided == actual) {
      return ret;
    } else {
      throw std::invalid_argument("Key does not exist");
    }
  }
};

template<class Key, class Value, class... Args>
Mapper(std::pair<Key, Value>, Args...) -> Mapper<std::pair<impl::MapValueType_t<Key>, impl::MapValueType_t<Value>>, Args...>;
}// namespace common::utils