#pragma once

#include <optional>

#include "task_log_entry.hpp"

namespace hhctrl::core::scheduler
{

class FileTaskLog
{
public:
  explicit FileTaskLog(std::string fname);
  void add(TaskLogEntry);
  void erase(const std::string&);
  std::optional<TaskLogEntry> find(const std::string&) const;
  bool exist(const std::string&) const;
  void update();

private:
  void load();
  void store();
private:
  std::string fname_;
};
}