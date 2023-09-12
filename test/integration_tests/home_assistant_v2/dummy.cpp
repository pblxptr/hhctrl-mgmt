////
//// Created by bielpa on 20.08.23.
////
//
//#include <catch2/catch_all.hpp>
//
//#include <boost/asio/io_context.hpp>
//#include <boost/asio/use_awaitable.hpp>
//#include <boost/asio/ip/tcp.hpp>
//#include <boost/asio/co_spawn.hpp>
//#include <boost/asio/connect.hpp>
//#include <async_mqtt/all.hpp>
//#include <spdlog/spdlog.h>
//
//void rethrow(std::exception_ptr ptr)
//{
//    if (ptr) {
//        std::rethrow_exception(ptr);
//    }
//}
//
//TEST_CASE("Client can connect to broker 2")
//{
//    auto ioc = boost::asio::io_context{};
//    auto ep = async_mqtt::endpoint<async_mqtt::role::client, async_mqtt::protocol::mqtt> {
//        async_mqtt::protocol_version::v3_1_1, ioc.get_executor()
//    };
//
//    // NOLINTBEGIN
//    boost::asio::co_spawn(ioc.get_executor(), [&ep, &ioc]() mutable -> boost::asio::awaitable<void> {
//
//        // Resolve
//        auto resolver = boost::asio::ip::tcp::resolver{ep.next_layer().get_executor()};
//        auto eps = co_await resolver.async_resolve("127.0.0.1", "1883", boost::asio::use_awaitable);
//
//        // Connect
//        co_await boost::asio::async_connect(
//                ep.next_layer(),
//                eps,
//                boost::asio::use_awaitable);
//
//        // Send connect packet
//        auto packet = async_mqtt::v3_1_1::connect_packet {
//                true,
//                15,
//                async_mqtt::allocate_buffer("some_unique_id_1234567"),
//                std::nullopt,
//                async_mqtt::allocate_buffer("test_user"), // credentials for my broker
//                async_mqtt::allocate_buffer("test")
//        };
//
//        if (auto system_error = co_await ep.send(std::move(packet), boost::asio::use_awaitable)) {
//            throw std::runtime_error{system_error.what()};
//        }
//
//        // Recv connack packet
//        if (async_mqtt::packet_variant packet_variant = co_await ep.recv(boost::asio::use_awaitable)) {
//            packet_variant.visit(
//                    async_mqtt::overload{
//                            [](const auto&) {
//                            }
//                    });
//        } else {
//            throw std::runtime_error{packet_variant.get<async_mqtt::system_error>().message()};
//        }
//
//        auto new_ep = std::move(ep);
//
//        // Receive
//        while (true) {
//            if (auto recv_packet = co_await new_ep.recv(boost::asio::use_awaitable)) {
//                auto type = recv_packet.type();
//                if (!type) {
//                    throw std::runtime_error{"Invalid packet"};
//                }
//                spdlog::get("async_mqtt_client")->debug("Packet received: {}", async_mqtt::control_packet_type_to_str(type.value()));
//            } else {
//                throw std::runtime_error{recv_packet.template get<async_mqtt::system_error>().message()};
//            }
//        }
//    }, rethrow);
//    // NOLINTEND
//
//    ioc.run();
//}
