//
// Created by bielpa on 25.08.23.
//

#include <async_mqtt/all.hpp>

namespace mgmt::home_assistant::v2
{
  namespace detail {
    void dump_packet(const async_mqtt::v3_1_1::publish_packet& pub_packet)
    {
      std::cout
        << "MQTT PUBLISH recv"
        << " pid:" << pub_packet.packet_id()
        << " topic:" << pub_packet.topic()
        << " payload:" << async_mqtt::to_string(pub_packet.payload())
        << " qos:" << pub_packet.opts().get_qos()
        << " retain:" << pub_packet.opts().get_retain()
        << " dup:" << pub_packet.opts().get_dup()
        << std::endl;
    }

    void dump_packet(const async_mqtt::v3_1_1::puback_packet& puback_packet)
    {
      std::cout
        << "MQTT PUBACK recv"
        << " pid:" << puback_packet.packet_id()
        << std::endl;
    }

    void dump_packet(const async_mqtt::v3_1_1::suback_packet& suback_packet)
    {

    }
  }
}
