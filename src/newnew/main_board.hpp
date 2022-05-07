#include <newnew/board.hpp>

namespace mgmt::device
{
  class MainBoard : public Board
  {
  public:
    explicit MainBoard(DeviceId_t id) : Board{std::move(id)}
      {}
    void restart()
    {

    }
    HardwareIdentity hardware_identity()
    {
      return HardwareIdentity {
        .model = "Henhouse controller",
        .revision = "R1",
        .serial_number = "S0000000"
      };
    }
  };
}