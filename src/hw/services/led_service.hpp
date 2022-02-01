#pragma once

#include <common/data/indicator.hpp>

namespace hw::services
{
  class LedService
  {
  public:
    virtual ~LedService() = default;
    virtual void set_state(common::data::IndicatorType, common::data::IndicatorState) const = 0;
    virtual common::data::IndicatorState get_state(common::data::IndicatorType) const = 0;
  };
}