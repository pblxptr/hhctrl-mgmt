#pragma once

#include <variant>

namespace hw::platform_device
{

template<class V, class T> //Todo: move to utility
concept VariantContains = requires(V v, T t)
{
  v = t;
};

class DeviceAttributes
{
  using Attribute_t = std::variant<
      std::string,
      std::uint32_t
    >;
public:
  template<class...T>
  DeviceAttributes(T&&... args)
  {
    auto add_attribute = [this](auto&& arg)
    {
      auto&& [key, val] = std::forward<decltype(arg)>(arg);

      set_attribute(std::forward<decltype(key)>(key), std::forward<decltype(val)>(val));
    };

    (add_attribute(std::forward<decltype(args)>(args)), ...);
  }

  template<class T>
  const auto& attribute(const std::string& key) const
  {
    const auto& val = attributes_.at(key);

    return std::get<T>(val);
  }

  template<class T>
  std::optional<T> find_attribute(const std::string& key) const
  {
    if (not attributes_.contains(key)) {
      return std::nullopt;
    }

    const auto& val = attributes_.at(key);

    if (not std::holds_alternative<T>(val)) {
      return std::nullopt;
    }

    return std::get<T>(val);
  }

  template<class T>
  void set_attribute(const std::string& key, T value) requires VariantContains<Attribute_t, T>
  {
    attributes_[key] = std::move(value);
  }

private:
  std::unordered_map<std::string, Attribute_t> attributes_;
};
}