#pragma once

#include <common/utils/enum_mapper.hpp>
#include <common/data/indicator.hpp>
#include <bci.pb.h>

namespace common::mapper {
  using IndicatorTypeMapper_t = common::utils::EnumMapper<>::MakeMapper<
  common::utils::Tie<common::data::IndicatorType::Status, bci::IndicatorType::Status>,
  common::utils::Tie<common::data::IndicatorType::Warning, bci::IndicatorType::Warning>,
  common::utils::Tie<common::data::IndicatorType::Maintenance, bci::IndicatorType::Maintenance>,
  common::utils::Tie<common::data::IndicatorType::Fault, bci::IndicatorType::Fault>
>;

using IndicatorStateMapper_t = common::utils::EnumMapper<>::MakeMapper<
  common::utils::Tie<common::data::IndicatorState::SteadyOn, bci::IndicatorState::SteadyOn>,
  common::utils::Tie<common::data::IndicatorState::SteadyOff, bci::IndicatorState::SteadyOff>
>;
}