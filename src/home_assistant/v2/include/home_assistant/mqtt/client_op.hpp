//
// Created by bielpa on 25.08.23.
//

#pragma once

#include <async_mqtt/all.hpp>

constexpr inline auto DefaultProtocolVersion = async_mqtt::protocol_version::v3_1_1;

using ProtocolVersion = async_mqtt::protocol_version;
using QOS = async_mqtt::qos;
using SubAckReturnCode = async_mqtt::suback_return_code;
using TopicSubAckResult = std::tuple<std::string, SubAckReturnCode>;
using SubscribeResult = std::tuple<std::error_code, std::vector<TopicSubAckResult>>;
using PacketId = std::uint32_t;
using PublishPacket = async_mqtt::v3_1_1::publish_packet;
using PublishAckPacket = async_mqtt::v3_1_1::puback_packet;
using SubscriptionAckPacket = async_mqtt::v3_1_1::suback_packet;

using ReceiveResult = std::variant<PublishPacket, PublishAckPacket>;

namespace detail {

}

namespace mgmt::home_assistant::v2
{

}
