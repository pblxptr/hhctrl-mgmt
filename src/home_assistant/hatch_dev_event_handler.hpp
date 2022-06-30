#include <common/event/event.hpp>
#include <boost/asio/awaitable.hpp>
#include <device/hatch.hpp>

class HatchDevEventHandler
{
public:
  using Hatch_t = mgmt::device::Hatch_t;

  boost::asio::awaitable<void> operator()()
  {

  }
};
