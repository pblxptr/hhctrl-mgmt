//
// Created by pp on 7/29/22.
//

#pragma once

#include <home_assistant/client_config.hpp>

#include <fmt/format.h>

namespace mgmt::app {
struct AppConfig
{
  std::string log_dir;
  std::string dtree_file;
  mgmt::home_assistant::adapter::EntityClientConfig entity_client_config;
};

inline std::string to_string(const AppConfig& config)
{
  using std::to_string;

  return fmt::format(
    "log_dir: {}\n",
    "dtree_file: {}\n"
    "entity_client_config: {}\n",
    config.log_dir,
    config.dtree_file,
    to_string(config.entity_client_config));
}

AppConfig load_config(const std::string& path);
std::string pretty_format_config(const mgmt::app::AppConfig& config);
}// namespace mgmt::app
