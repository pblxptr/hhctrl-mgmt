#include <home_assistant/entity_factory.hpp>
#include <home_assistant/mqtt/cover.hpp>

namespace mgmt::home_assistant::mqttc {
using Cover_t = std::decay_t<decltype(std::declval<EntityFactory>().create_cover(std::declval<std::string>()))>;
using BinarySensor_t = std::decay_t<decltype(std::declval<EntityFactory>().create_binary_sensor(std::declval<std::string>()))>;
using Button_t = std::decay_t<decltype(std::declval<EntityFactory>().create_button(std::declval<std::string>()))>;
}// namespace mgmt::home_assistant::mqttc