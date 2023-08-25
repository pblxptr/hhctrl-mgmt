//
// Created by bielpa on 24.08.23.
//

#pragma once

#include <system_error>
#include <boost/system/errc.hpp>

namespace mgmt::home_assistant::v2
{
  enum class EntityError {
    Timeout = 1 ,
    NoService,
    Disconnected,
    PublishError,
    SubscriptionError,
    UnknownPacket
};

  namespace detail {
    struct EntityErrorCategory : std::error_category
    {
      const char* name() const noexcept override
      {
        return "entity";
      }

      std::string message(int error) const override
      {
        switch (static_cast<EntityError>(error)) {
          case EntityError::Timeout:
            return "timeout";
          case EntityError::NoService:
            return "no_service";
          case EntityError::Disconnected:
            return "disconnected";
          case EntityError::PublishError:
            return "publish_error";
          case EntityError::SubscriptionError:
          return "subscription_error";
          default:
            return "unrecognized error";
        }
      }
    };

    static const inline auto EntityCategoryInstance = EntityErrorCategory{};
  } // namespace detail

  std::error_code make_error_code(EntityError error)
  {
    return {static_cast<int>(error), detail::EntityCategoryInstance};
  }
} // namespace mgmt::home_assistant::v2


namespace std
{
  template <>
  struct is_error_code_enum<mgmt::home_assistant::v2::EntityError> : true_type {};
} // namespace std


