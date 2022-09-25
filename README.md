# HHCtrl Management
This project contains logic for managing hh. Manages software tasks and provides convenient interfaces to hardware.

## Build status ## 

[![.github/workflows/build.yml](https://github.com/pblxptr/hhctrl-mgmt/actions/workflows/build.yml/badge.svg)](https://github.com/pblxptr/hhctrl-mgmt/actions/workflows/build.yml)

[![.github/workflows/quality.yml](https://github.com/pblxptr/hhctrl-mgmt/actions/workflows/quality.yml/badge.svg)](https://github.com/pblxptr/hhctrl-mgmt/actions/workflows/quality.yml)

## Necessary Dependencies ##
A C++20 support is required as the code highly utlizes new features like concepts and chrono datetime and calendar functionalities.

- GCC 10+ 
- Boost 1.75+ (required Boost.Json)
- spdlog
- fmt

## Future improvements ##
- Full async support through coroutines
