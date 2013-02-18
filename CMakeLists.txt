cmake_minimum_required(VERSION 1.6)
project(MGZ_DIFF)
set(MGZ_DIFF_VERSION 0.0.1 GLOBAL)
set(MGZ_DIFF_LVERSION 0 GLOBAL)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__WIN32__")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__WIN32__")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__APPLE__")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__APPLE__")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__UNIX__")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__UNIX__")
endif()

if(MINGW)
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
endif()

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

if(WIN32 OR MGZ_PATH)
  if(NOT MGZ_PATH)
    message(FATAL_ERROR "Please use -DMGZ_PATH:PATH=/path/to/mgz_utils")
  endif()

  find_path(MGZ_IO_INCLUDE_DIRS "io/file.h" PATHS "${MGZ_PATH}/include")
  find_path(MGZ_IO_LIBRARY_DIRS "mgz-io.dll" PATHS "${MGZ_PATH}/lib")
  find_path(MGZ_SECURITY_INCLUDE_DIRS "security/crc32.h" PATHS "${MGZ_PATH}/include")
  find_path(MGZ_SECURITY_LIBRARY_DIRS "mgz-security.dll" PATHS "${MGZ_PATH}/lib")

  if(NOT MGZ_IO_INCLUDE_DIRS OR NOT MGZ_IO_LIBRARY_DIRS OR NOT MGZ_SECURITY_INCLUDE_DIRS OR NOT MGZ_SECURITY_LIBRARY_DIRS)
    message(FATAL_ERROR "Please verify your -DMGZ_PATH:PATH=/path/to/mgz_utils")
  endif()

  set(MGZ_SECURITY_LIBRARIES 
    mgz-util 
    mgz-security
    )
  set(MGZ_IO_LIBRARIES 
    mgz-util 
    mgz-regex 
    mgz-security 
    mgz-xml 
    mgz-io)
else()
	pkg_search_module(MGZ_IO REQUIRED mgz-io)
	pkg_search_module(MGZ_SECURITY REQUIRED mgz-security)
endif()

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
if(NOT WIN32)
add_subdirectory(bin)
endif()
add_subdirectory(ext)
add_subdirectory(test)
