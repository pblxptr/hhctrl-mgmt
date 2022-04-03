#pragma once

#include <string>

namespace mgmt::board_ctrl
{
  struct BoardInfo
  {
    std::string model;
    std::string hardware_revision;
    std::string serial_number;
  };
}