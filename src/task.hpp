#pragma once

#include <string>
#include <boost/uuid/uuid.hpp>
#include <functional>
#include <memory>
#include <chrono>

namespace hhctrl::core::scheduler
{
using Id_t = boost::uuids::uuid;
using Timepoint_t = std::chrono::time_point<std::chrono::system_clock>;
using TaskHandler_t = std::function<void()>;

class Task
{
public:
  virtual ~Task() = default;
  virtual const std::string& name() const = 0;
  virtual void install() = 0;
  virtual void set_expiry(const Timepoint_t&) = 0;
  virtual void reconfigure() = 0;
  virtual bool is_reinstallable() const = 0;
  virtual Timepoint_t expiry() const = 0;
  virtual std::string to_string() const = 0;
};
}