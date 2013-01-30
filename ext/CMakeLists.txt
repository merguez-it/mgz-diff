cmake_minimum_required (VERSION 2.8)
project(MGZ_DIFF_EXTERN)

include_directories(
  ${MGZ_DIFF_EXTERN_SOURCE_DIR}/gtest-1.6.0/include
  ${MGZ_DIFF_EXTERN_SOURCE_DIR}/gtest-1.6.0
  )

add_library(gtest ${MGZ_DIFF_EXTERN_SOURCE_DIR}/gtest-1.6.0/src/gtest-all.cc)
add_library(gtest_main ${MGZ_DIFF_EXTERN_SOURCE_DIR}/gtest-1.6.0/src/gtest_main.cc)

