#pragma once

#include <common/traits/variant_traits.hpp>
#include <variant>

namespace hw::platform_device
{
#
class DeviceAttribute
{
  using Value_t = std::variant<
      std::string,
      std::uint32_t
    >;
public:
  DeviceAttribute() = default;

  template<class T>
  explicit DeviceAttribute(T value)
    : value_{std::move(value)}
  {}

  template<class T>
  bool is() const
  {
    return std::holds_alternative<T>(value_);
  }

  template<class T>
  T as() const
  {
    if (not is<T>()) {
      throw std::runtime_error("Cannot get attribute. Requested type is not compatible");
    }

    return std::get<T>(value_);
  }

  template<class T>
  T get() const
  {
    return std::get<T>(value_);
  }

private:
  Value_t value_;
};

class DeviceAttributes
{
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

    return val.get<T>();
  }

  template<class T>
  std::optional<T> find_attribute(const std::string& key) const
  {
    if (not attributes_.contains(key)) {
      return std::nullopt;
    }

    const auto& val = attributes_.at(key);

    if (not val.is<T>()) {
      return std::nullopt;
    }

    return val.get<T>();
  }

  template<class T>
  void set_attribute(const std::string& key, T&& value)
  {
    attributes_[key] = DeviceAttribute{std::forward<T>(value)};
  }

  decltype(auto) begin() const
  {
    return attributes_.begin();
  }

  decltype(auto) end() const
  {
    return attributes_.end();
  }

private:
  std::unordered_map<std::string, DeviceAttribute> attributes_;
};

inline std::string to_string(const DeviceAttribute& attribute)
{
  using std::to_string;

  if (attribute.is<std::string>()) {
    return attribute.get<std::string>();
  }
  else if (attribute.is<std::uint32_t>()) {
    return to_string(attribute.get<std::uint32_t>());
  }
  else {
    return "";
  }
}
}