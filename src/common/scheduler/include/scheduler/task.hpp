#pragma once

#include <string>
#include <boost/uuid/uuid.hpp>
#include <memory>
#include <chrono>

// TODO(pp): Consider changing to base class instead of interface, as all types share some similar methods to return
// e.g. id, owner and so on.

namespace common::scheduler {
class ITask
{
public:
  using Id_t = boost::uuids::uuid;
  using Timepoint_t = std::chrono::time_point<std::chrono::system_clock>;

  ITask() = default;
  //movable
  ITask(ITask&&) noexcept = default;
  ITask& operator=(ITask&&) noexcept = default;
  //copyable
  ITask(const ITask&) = default;
  ITask& operator=(const ITask&) = default;

  virtual ~ITask() = default;

  virtual const Id_t& id() const = 0;
  virtual const std::string& owner() const = 0;
  virtual void activate() = 0;
  virtual Timepoint_t expiry() const = 0;
  virtual void set_expiry(Timepoint_t) = 0;
  virtual std::string to_string() const = 0;
};

struct TaskInfo
{
  ITask::Id_t id;
  std::string owner;
  ITask::Timepoint_t expiry;
};
}// namespace common::scheduler
