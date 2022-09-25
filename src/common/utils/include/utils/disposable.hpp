#pragma once

namespace common::utils {
template<class T>
class Disposable : public T
{
  using OnDispose = std::function<void(const T&)>;

public:
  template<class... Args>
  Disposable(OnDispose on_dispose, Args&&... args)
    : on_dispose_{ std::move(on_dispose) }
    , T{ std::forward<Args>(args)... }
  {}

  ~Disposable()
  {
    on_dispose_(*this);
  }

private:
  OnDispose on_dispose_;
};

template<class T, class... Args>
auto make_disposable(Args&&... args)
{
  return std::make_unique<DisposableDevice<T>>(std::forward<Args>(args)...);
}
}// namespace common::utils