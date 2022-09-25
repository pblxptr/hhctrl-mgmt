include(FetchContent)

FetchContent_Declare(
  range-v3
  GIT_REPOSITORY https://github.com/ericniebler/range-v3.git
  GIT_TAG 689b4f3da769fb21dd7acf62550a038242d832e5
)

FetchContent_GetProperties(range-v3)
if (NOT range-v3_POPULATED)
  FetchContent_Populate(range-v3)
endif ()

add_library(range-v3 INTERFACE)

target_include_directories(range-v3
  INTERFACE ${range-v3_SOURCE_DIR}/include
)
