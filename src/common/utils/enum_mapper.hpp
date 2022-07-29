#pragma once

namespace common::utils {
template<auto T1, auto T2>
struct Tie
{
  constexpr static decltype(T1) t1 = T1;
  constexpr static decltype(T2) t2 = T2;
};

template<class = void>
class EnumMapper;

template<class Config>
class EnumMapper
{
public:
  template<class... T>
  using MakeConfig = std::tuple<T...>;

  template<class... T>
  using MakeMapper = EnumMapper<MakeConfig<T...>>;

  template<class Destination, class Source>
  static auto map(const Source& source)
  {
    static_assert(!std::is_same_v<Destination, Source>, "Destination and Source types cannot be the same.");

    return mape_types<Destination>(source, std::make_integer_sequence<uint32_t, std::tuple_size_v<Config>>{});
  }

  template<class Destination, class Source>
  static auto map_safe(const Source& source)
  {
    auto result = map<Destination, Source>(source);

    if (!result) {
      throw std::runtime_error("Source type cannot be mapped. Destination value not found in configuration.");
    }

    return *result;
  }

private:
  template<bool Flag = false>
  static constexpr void type_mismatch()
  {
    static_assert(Flag, "EnumMapper cannot perform mapping.");
  }

  template<class Destination, class Source, uint32_t... Is>
  static auto mape_types(const Source& source, std::integer_sequence<uint32_t, Is...>)
  {
    auto destination = std::optional<Destination>{};

    (tryToMap<Is, Destination, Source>(destination, source) || ...);

    return destination;
  }

  template<auto Idx, class Destination, class Source>
  static bool tryToMap(std::optional<Destination>& ret_val, const Source& src)
  {
    using SelectedTie = std::tuple_element_t<Idx, Config>;
    using RawDst_t = std::decay_t<Destination>;
    using RawSrc_t = std::decay_t<Source>;
    using RawT1_t = std::decay_t<decltype(SelectedTie::t1)>;
    using RawT2_t = std::decay_t<decltype(SelectedTie::t2)>;

    static_assert(
      std::is_same_v<RawDst_t, RawT1_t> || std::is_same_v<RawDst_t, RawT2_t>, "Destination type does not belong to the configuration.");

    if constexpr (std::is_same_v<RawSrc_t, RawT1_t>) {
      if (SelectedTie::t1 == src) {
        ret_val.emplace(SelectedTie::t2);
        return true;
      }
    } else if constexpr (std::is_same_v<RawSrc_t, RawT2_t>) {
      if (SelectedTie::t2 == src) {
        ret_val.emplace(SelectedTie::t1);
        return true;
      }
    } else {
      type_mismatch();
    }

    return false;
  }
};
}// namespace common::utils