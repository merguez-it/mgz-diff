cmake_minimum_required(VERSION 2.6)
project(MGZ_DIFF_LIB)

if(WIN32)
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/mgz-diff.rc.in
    ${MGZ_DIFF_OUTPUT_TEMP_DIR}/mgz-diff.rc
    )
  set(MGZ_DIFF_RC ${MGZ_DIFF_OUTPUT_TEMP_DIR}/mgz-diff.rc)
endif()

set(MGZ_DIFF_LIB_SRC 
  ${MGZ_DIFF_LIB_SOURCE_DIR}/diff-delta.c
  ${MGZ_DIFF_LIB_SOURCE_DIR}/patch-delta.c
  ${MGZ_DIFF_LIB_SOURCE_DIR}/isolate.c
  ${MGZ_DIFF_LIB_SOURCE_DIR}/mgzdiff.cc
  ${MGZ_DIFF_RC}
  )

add_library(mgz-diff-s STATIC ${MGZ_DIFF_LIB_SRC})
add_library(mgz-diff SHARED ${MGZ_DIFF_LIB_SRC})
target_link_libraries(mgz-diff ${MGZ_SECURITY_LIBRARIES} ${MGZ_IO_LIBRARIES})
set_target_properties(mgz-diff-s
  PROPERTIES
  OUTPUT_NAME "mgz-diff"
  )
if(WIN32)
  set_target_properties(mgz-diff
    PROPERTIES
    PREFIX ""
    IMPORT_SUFFIX ${CMAKE_IMPORT_LIBRARY_SUFFIX}
    )
  set_target_properties(mgz-diff-s
    PROPERTIES
    PREFIX ""
    SUFFIX .lib
    IMPORT_SUFFIX ${CMAKE_IMPORT_LIBRARY_SUFFIX}
    )
endif()
set_target_properties(mgz-diff
  PROPERTIES 
  SOVERSION ${MGZ_DIFF_VERSION}
  VERSION ${MGZ_DIFF_LVERSION}
  )

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/mgz-diff.pc.in
  ${MGZ_DIFF_OUTPUT_TEMP_DIR}/mgz-diff.pc
  @ONLY
  )

install(DIRECTORY ${MGZ_DIFF_SOURCE_DIR}/include/mgz
  DESTINATION include
  PATTERN "internal" EXCLUDE
  )
install(TARGETS mgz-diff mgz-diff-s
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION lib
  )
install(FILES ${MGZ_DIFF_OUTPUT_TEMP_DIR}/mgz-diff.pc DESTINATION lib/pkgconfig)
