cmake_minimum_required (VERSION 2.8)
project(MGZ_DIFF_TESTS)

set(MGZ_TEST_PATH ${CMAKE_CURRENT_SOURCE_DIR})
configure_file(
  ${MGZ_DIFF_TESTS_SOURCE_DIR}/config-test.h.in
  config-test.h
  )

include_directories(
  ${MGZ_DIFF_EXTERN_SOURCE_DIR}/gtest-1.6.0/include
  ${MGZ_DIFF_SOURCE_DIR}/include
  ${CMAKE_CURRENT_BINARY_DIR}
  )

set(TESTS_LIBS
  mgz-diff-s
  gtest
  gtest_main
  )

add_executable(dummy_unittest "dummy_unittest.cc")
target_link_libraries(dummy_unittest ${TESTS_LIBS})
add_test(DUMMY_UNITTEST dummy_unittest)

