#include <static/main_board.hpp>

namespace mgmt::device
{
  HardwareIdentity MainBoard::hardware_identity() const
  {
    return HardwareIdentity {
      .model = "HenhouseCtrl",
      .revision = "R1",
      .serial_number = "serial_number"
    };
  }
}