cmake_minimum_required(VERSION 2.6)
project(MGZ_DIFF)
set( MGZ_DIFF_VERSION 0.0.1 )

include(FindDoxygen)
include(CheckFunctionExists) 
include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)
include(FindPkgConfig)


if(DEBUG)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -g -O0")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -O0")
  set(CMAKE_VERBOSE_MAKEFILE ON)
endif(DEBUG)

set(MGZ_DIFF_OUTPUT_TEMP_DIR ${CMAKE_BINARY_DIR}/.build)
file(MAKE_DIRECTORY ${MGZ_DIFF_OUTPUT_TEMP_DIR})

CHECK_FUNCTION_EXISTS(pthread_create HAVE_PTHREAD)
CHECK_C_COMPILER_FLAG(-pthread C_HAS_PTHREAD)
CHECK_CXX_COMPILER_FLAG(-pthread CXX_HAS_PTHREAD)

if(C_HAS_PTHREAD)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")
elseif(CXX_HAS_PTHREAD)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")
elseif(HAVE_PTHREAD)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lpthread")
endif()

include_directories(
  ${MGZ_DIFF_SOURCE_DIR}/src
  ${MGZ_DIFF_SOURCE_DIR}/include
  )

pkg_search_module(MGZ_IO REQUIRED mgz-io)
pkg_search_module(MGZ_SECURITY REQUIRED mgz-security)

include_directories(${MGZ_IO_INCLUDE_DIRS})
link_directories(${MGZ_IO_LIBRARY_DIRS})

include_directories(${MGZ_SECURITY_INCLUDE_DIRS})
link_directories(${MGZ_SECURITY_LIBRARY_DIRS})

# Documentation
if(DOXYGEN_FOUND)
  find_program(GRAPHVIZ
    dot
    )
  if("${GRAPHVIZ}" STREQUAL "GRAPHVIZ-NOTFOUND")
    message(STATUS "GraphViz not found!")
    set(HAVE_DOT NO)
  else()
    message(STATUS "Found GraphViz: ${GRAPHVIZ}")
    set(HAVE_DOT YES)
  endif()
  configure_file(
    ${MGZ_DIFF_SOURCE_DIR}/Doxyfile.in 
    ${MGZ_DIFF_OUTPUT_TEMP_DIR}/Doxyfile
    )

  set(DOXYGEN_INPUT ${MGZ_DIFF_OUTPUT_TEMP_DIR}/Doxyfile)
  add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT})
endif()

enable_testing()
add_subdirectory(src)
add_subdirectory(bin)
add_subdirectory(ext)
add_subdirectory(test)
