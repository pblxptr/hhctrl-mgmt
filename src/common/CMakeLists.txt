cmake_minimum_required(VERSION 3.12)

set(COMPONENTS
  command
  coro
  event
  executor
  logger
  scheduler
  traits
  utils
)

foreach (COMPONENT IN LISTS COMPONENTS)
  add_subdirectory(${COMPONENT})
endforeach()
