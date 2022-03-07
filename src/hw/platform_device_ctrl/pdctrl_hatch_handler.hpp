#pragma once

#include <iconect/pdci/pdci.pb.h>


template<class DriverInterface>
class DriverControlAdapter
{
public:
  void add_compatible_driver_id()
};

namespace hw::pdctrl
{
  class PdciHatchControlAdapter : public DriverControlAdapter<HatchDriver>
  {
  public:
    void add_driver_id()


    template<class Builder>
    void setup(Builder& builder)
    {
      builder.template add_consumer<pdci::GetStatusReq>([this](auto& context)
        -> awaitable<void> { co_await handle(context); }));

      builder.template add_consumer<pdci::OpenHatchReq>([this](auto& context)
        -> awaitable<void> { co_await handle(context); }));

      builder.template add_consumer<pdci::CloseHatchReq>([this](auto& context)
        -> awaitable<void> { co_await handle(context); }));
    }
  private:
    boost::asio::awaitable<void> handle(icon::MessageContext<bci::GetStatusReq>&)
    {
      auto driver = get_driver<hw::drivers::HatchDriver>("driver_rt_id")->open()
    }
    boost::asio::awaitable<void> handle(icon::MessageContext<bci::OpenHatchReq>&) { co_return; }
    boost::asio::awaitable<void> handle(icon::MessageContext<bci::CloseHatchReq>&) { co_return; }
  };
}
