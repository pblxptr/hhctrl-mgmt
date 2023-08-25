//
// Created by bielpa on 24.08.23.
//

#pragma once

#include <system_error>
#include <boost/system/errc.hpp>

namespace mgmt::home_assistant::v2
{
  enum class ClientError { Timeout = 1 , NoService, UnknownPacket } ;

  namespace detail {
    struct MqttClientErrorCategory : std::error_category
    {
      const char* name() const noexcept override
      {
        return "mqtt_client";
      }

      std::string message(int error) const override
      {
        switch (static_cast<ClientError>(error)) {
          case ClientError::Timeout:
            return "timeout";
          case ClientError::NoService:
            return "no_service";
          case ClientError::UnknownPacket:
            return "unknown_packet";
          default:
            return "unrecognized error";
        }
      }
    };

    ClientError map_error_code(boost::system::error_code error_code)
    {
      switch (error_code.value()) {
      case boost::system::errc::timed_out:
        return ClientError::Timeout;
      case boost::system::errc::no_such_file_or_directory:
        return ClientError::NoService;
      default:
        throw std::runtime_error{ error_code.message() };
      }
    }

    static const inline auto MqttClientCategoryInstance = MqttClientErrorCategory{};
  } // namespace detail

  std::error_code make_error_code(ClientError error)
  {
    return {static_cast<int>(error), detail::MqttClientCategoryInstance};
  }
} // namespace mgmt::home_assistant::v2


namespace std
{
  template <>
  struct is_error_code_enum<mgmt::home_assistant::v2::ClientError> : true_type {};
} // namespace std


