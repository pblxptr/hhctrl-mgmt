cmake_minimum_required(VERSION 3.12)

option(USE_SANITIZERS, "Use sanitizers (address, undefined)" OFF)

if (USE_SANITIZERS)
  add_compile_options(-fsanitize=undefined,address )
  add_link_options(-fsanitize=undefined,address)
endif()
