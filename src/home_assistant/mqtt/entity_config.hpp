#pragma once

#include <string>
#include <string_view>
#include <boost/json.hpp>

namespace mgmt::home_assistant::mqttc
{
  class EntityConfig
  {
  public:
    explicit EntityConfig(std::string unique_id)
      : unique_id_{std::move(unique_id)}
    {}

    auto unique_id() const
    {
      return unique_id_;
    }

    template<class Value>
    void set(std::string_view key, Value&& value)
    {
      object_[key.data()] = std::forward<Value>(value);
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
}