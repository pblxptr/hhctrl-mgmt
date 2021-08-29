#pragma once

#include <string>
#include <boost/uuid/uuid.hpp>
#include <memory>
#include <chrono>

namespace hhctrl::core::scheduler
{
class Task
{
public:
  using Id_t = boost::uuids::uuid;
  using Timepoint_t = std::chrono::time_point<std::chrono::system_clock>;

  ~Task() = default;
  virtual const Id_t& id() const = 0;
  virtual const std::string& owner() const = 0;
  virtual void activate() = 0;
  virtual Timepoint_t expiry() const = 0;
  virtual void set_expiry(Timepoint_t) = 0;
  virtual std::string to_string() const = 0;
};
}