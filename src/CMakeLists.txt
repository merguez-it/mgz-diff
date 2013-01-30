cmake_minimum_required(VERSION 2.6)
project(MGZ_DIFF_LIB)

set(MGZ_DIFF_LIB_SRC 
  ${MGZ_DIFF_LIB_SOURCE_DIR}/diff-delta.c
  ${MGZ_DIFF_LIB_SOURCE_DIR}/patch-delta.c
  ${MGZ_DIFF_LIB_SOURCE_DIR}/isolate.c
  )
add_library(mgz-diff-s STATIC ${MGZ_DIFF_LIB_SRC})
add_library(mgz-diff SHARED ${MGZ_DIFF_LIB_SRC})

