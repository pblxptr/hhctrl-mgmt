#pragma once

#include <string>
#include <string_view>
#include <boost/json.hpp>

#include <home_assistant/device_identity.hpp>

namespace mgmt::home_assistant::mqttc {
class EntityConfig
{
public:
  explicit EntityConfig(std::string unique_id)
    : unique_id_{ std::move(unique_id) }
  {}

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


  auto parse()
  {
    object_["unique_id"] = unique_id_;

    return boost::json::serialize(object_);
  }

private:
  std::string unique_id_;
  boost::json::object object_;
};

namespace helper {
  inline auto entity_config_basic_device(const mgmt::home_assistant::DeviceIdentity& identity)
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
