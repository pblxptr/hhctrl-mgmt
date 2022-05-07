#pragma once

#include <concepts>

#include <newnew/device.hpp>

namespace mgmt::device
{
  template<class T> requires std::derived_from<T, Device>
  class DisposableDevice : public T
  {
    using OnDispose = std::function<void(const T&)>;
  public:
    template<class...Args>
    DisposableDevice(OnDispose on_delete, Args&&... args)
      : on_dispose_{std::move(on_delete)}
      , T{std::forward<Args>(args)...}
    {}

    DisposableDevice(const DisposableDevice&) = default;
    DisposableDevice(DisposableDevice&&) = default;
    DisposableDevice& operator=(const DisposableDevice&) = default;
    DisposableDevice& operator=(DisposableDevice&&) = default;

    ~DisposableDevice()
    {
      on_dispose_(*this);
    }
  private:
    OnDispose on_dispose_;
  };

  template<class T, class...Args>
  auto make_disposable_device(Args&&...args)
  {
    return std::make_unique<DisposableDevice<T>>(std::forward<Args>(args)...);
  }
}