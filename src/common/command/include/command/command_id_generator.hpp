#pragma once

#include <cstddef>
#include <atomic>

#include <command/command_id.hpp>

namespace common::command {
class CommandIdGenerator
{
public:
  template<class Command>
  static CommandId_t get()
  {
    return id<std::decay_t<Command>>();
  }

private:
  template<class Command>
  static CommandId_t id()
  {
    static CommandId_t id = ids_counter_++;

    return id;
  }

private:
  static inline std::atomic<CommandId_t> ids_counter_{};
};
}// namespace common::command