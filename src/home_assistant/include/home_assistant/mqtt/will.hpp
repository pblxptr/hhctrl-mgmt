#pragma once

#include <string_view>

namespace mgmt::home_assistant::mqttc {
struct Will
{
  std::string_view topic;
  std::string_view payload;
};
}
