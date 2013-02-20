cmake_minimum_required (VERSION 2.8)
project(MGZ_DIFF_TESTS)

set(MGZ_TEST_PATH ${CMAKE_CURRENT_SOURCE_DIR})
configure_file(
  ${MGZ_DIFF_TESTS_SOURCE_DIR}/config-test.h.in
  config-test.h
  )

include_directories(
  ${MGZ_IO_INCLUDE_DIRS}
  ${MGZ_SECURITY_INCLUDE_DIRS}
  ${MGZ_DIFF_EXTERN_SOURCE_DIR}/gtest-1.6.0/include
  ${MGZ_DIFF_SOURCE_DIR}/include
  ${CMAKE_CURRENT_BINARY_DIR}
  )

set(TESTS_LIBS
  mgz-diff-s
  ${MGZ_SECURITY_LIBRARIES}
  ${MGZ_IO_LIBRARIES}
  gtest
  gtest_main
  )

add_executable(mgzdiff_unittest "mgzdiff_unittest.cc")
target_link_libraries(mgzdiff_unittest ${TESTS_LIBS})
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set_target_properties(mgzdiff_unittest PROPERTIES LINK_FLAGS "-static-libstdc++ -static-libgcc")
endif()
add_test(MGZDIFF_UNITTEST mgzdiff_unittest)
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set_tests_properties(MGZDIFF_UNITTEST PROPERTIES ENVIRONMENT "PATH=${MGZ_IO_LIBRARY_DIRS}")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set_tests_properties(MGZDIFF_UNITTEST PROPERTIES ENVIRONMENT "DYLD_LIBRARY_PATH=$ENV{DYLD_LIBRARY_PATH}:${MGZ_IO_LIBRARY_DIRS}")
else()
  set_tests_properties(MGZDIFF_UNITTEST PROPERTIES ENVIRONMENT "LD_LIBRARY_PATH=$ENV{LD_LIBRARY_PATH}:${MGZ_IO_LIBRARY_DIRS}")
endif()
