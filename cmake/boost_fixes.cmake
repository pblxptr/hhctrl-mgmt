if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  add_compile_definitions(BOOST_ASIO_HAS_CO_AWAIT)
  add_compile_definitions(BOOST_ASIO_HAS_STD_COROUTINE)
endif()
