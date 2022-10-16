#pragma once

#include <string>
#include <fstream>
#include <sys/types.h>
#include <unistd.h>

namespace common::utils::proc
{
  inline std::string current_name()
  {
    auto file = std::ifstream {"/proc/self/comm"};
    auto proc_name = std::string{};
    std::getline(file, proc_name);

    return proc_name;
  }

  inline pid_t current_pid()
  {
    return getpid();
  }
}
