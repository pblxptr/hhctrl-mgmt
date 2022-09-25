#pragma once

#include <unordered_map>
#include <string_view>
#include <ranges>
#include <algorithm>
#include <string>
#include <fmt/format.h>
#include <catch2/catch_all.hpp>
#include <catch2/catch_session.hpp>

struct TestOption
{
  std::string name;
  std::string hint;
  std::string description;

  auto operator<=>(const TestOption&) const = default;
};

class TestConfig
{
  TestConfig() = default;

public:
  TestConfig(const TestConfig&) = delete;
  TestConfig& operator=(const TestConfig&) = delete;
  TestConfig(TestConfig&&) = delete;
  TestConfig& operator=(TestConfig&&) = delete;

public:
  static auto& get()
  {
    static TestConfig instance;

    return instance;
  }

  void add_option(const TestOption& option)
  {
    if (std::ranges::count_if(options_, [&option](auto&& opt) {
          return option.name == opt.name;
        })) {
      throw std::runtime_error(fmt::format("Option: {} already exists", option.name));
    }

    options_.push_back(option);
  }

  std::optional<std::string> option_value(std::string_view option_name) const
  {
    if (not options_placeholders_.contains(option_name.data())) {
      return std::nullopt;
    }

    auto option = options_placeholders_.at(option_name.data());

    if (option.empty()) {
      return std::nullopt;
    }

    return option;
  }

  template<class Session>
  void apply(Session& session)
  {
    using namespace Catch::Clara;

    auto cli = session.cli();

    for (auto& option : options_) {
      cli |= Opt(options_placeholders_[option.name], option.hint)[option.name](option.description);
    }
    session.cli(cli);
  }

private:
  std::vector<TestOption> options_;
  std::unordered_map<std::string, std::string> options_placeholders_;
};
