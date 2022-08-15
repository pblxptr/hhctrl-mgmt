cmake_minimum_required(VERSION 3.12)

set(COMPONENTS
  command
  #scheduler
)

foreach (COMPONENT IN LISTS COMPONENTS)
  add_subdirectory(${COMPONENT})
endforeach()


add_library(common_tests INTERFACE)
foreach (COMPONENT IN LISTS COMPONENTS)
  add_dependencies(common_tests ${COMPONENT}_tests)
  target_link_libraries(common_tests INTERFACE $<TARGET_OBJECTS:${COMPONENT}_tests>)
endforeach()

