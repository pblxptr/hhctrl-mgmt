//
// Created by pp on 7/24/22.
//

#pragma once

#include <string>
#include <utility>

namespace mgmt::home_assistant::mqttc {
class EntityError
{
public:
  enum class Code { Undefined,
    Disconnected };

  explicit EntityError(Code code, std::string message = {})
    : code_{ code }
    , message_{ std::move(message) }
  {}

  Code code() const
  {
    return code_;
  }

  const std::string& message() const
  {
    return message_;
  }

private:
  Code code_;
  std::string message_;
};
}// namespace mgmt::home_assistant::mqttc