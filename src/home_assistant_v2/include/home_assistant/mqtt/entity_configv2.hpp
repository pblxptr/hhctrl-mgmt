#pragma once

#include <string>
#include <string_view>
#include <boost/json.hpp>

#include <home_assistant/mqtt/device_identity.hpp>

namespace mgmt::home_assistant::v2 {
class EntityConfig
{
public:
  EntityConfig() = default;

  template<class Value>
  void set(std::string_view key, const Value& value)
  {
    if (object_.contains(key.data())) {
      return;
    }
    set_override(key, value);
  }

  template<class Value>
  void set_override(std::string_view key, Value&& value)
  {
    object_[key.data()] = std::forward<Value>(value);
  }

  template<class Value>
  void set_override_if_not_null(std::string_view key, const Value& value)
  {
    if (object_.contains(key.data()) && object_.at(key.data()).kind() == boost::json::kind::null) {
      return;
    }

    set_override(key, value);
  }

  bool contains(std::string_view key) const
  {
      return object_.contains(key.data());
  }

  std::optional<std::string> get(std::string_view key) const
  {
      if (!contains(key)) {
          return std::nullopt;
      }

      return boost::json::value_to<std::string>(object_.at(key.data()));
  }

  auto parse()
  {
    return boost::json::serialize(object_);
  }

  static std::optional<EntityConfig> from_json(const std::string& json)
  {
    unsigned char buf[ 4096 ];
    auto mem_resource = boost::json::static_resource { buf };

    auto value = boost::json::parse(json);

    if (!value.is_object()) {
        return std::nullopt;
    }

    return EntityConfig{std::move(value).as_object()};
  }

private:
    explicit EntityConfig(boost::json::object object)
      : object_{std::move(object)}
    {}

private:
  boost::json::object object_;
};

namespace helper {
  inline auto entity_config_basic_device(const DeviceIdentity& identity)
  {
    return boost::json::value{
      { "identifiers", boost::json::value_from(std::vector<std::string>{ identity.serial_number }) },
      { "manufacturer", identity.manufacturer },
      { "model", identity.model },
      { "name", fmt::format("{} {} {}", identity.manufacturer, identity.model, identity.hw_revision) },
      { "sw_version", identity.sw_revision }
    };
  }

}// namespace helper
}// namespace mgmt::home_assistant::mqttc
