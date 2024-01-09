include(FetchContent)

option(EXPECTED_BUILD_TESTS "" OFF)

FetchContent_Declare(
    expected-src
    GIT_REPOSITORY https://github.com/TartanLlama/expected
    GIT_TAG v1.1.0
)

FetchContent_MakeAvailable(expected-src)
