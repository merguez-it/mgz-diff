cmake_minimum_required(VERSION 2.6)
project(MGZ_DIFF_LIB)

set(MGZ_DIFF_LIB_SRC 
  ${MGZ_DIFF_LIB_SOURCE_DIR}/diff-delta.c
  ${MGZ_DIFF_LIB_SOURCE_DIR}/patch-delta.c
  ${MGZ_DIFF_LIB_SOURCE_DIR}/isolate.c
  ${MGZ_DIFF_LIB_SOURCE_DIR}/mgzdiff.cc
  )

add_library(mgz-diff-s STATIC ${MGZ_DIFF_LIB_SRC})
add_library(mgz-diff SHARED ${MGZ_DIFF_LIB_SRC})

target_link_libraries(mgz-diff-s ${MGZ_IO_LIBRARIES} )
target_link_libraries(mgz-diff ${MGZ_IO_LIBRARIES} )
target_link_libraries(mgz-diff-s ${MGZ_SECURITY_LIBRARIES} )
target_link_libraries(mgz-diff ${MGZ_SECURITY_LIBRARIES} )

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/mgz-diff.pc.in
  ${MGZ_DIFF_OUTPUT_TEMP_DIR}/mgz-diff.pc
  @ONLY
)
  
set(MGZ_DIFF_HEADERS 
  ${MGZ_DIFF_SOURCE_DIR}/include/diff.h
  ${MGZ_DIFF_SOURCE_DIR}/include/mgzdiff.h
)


install(FILES ${MGZ_DIFF_HEADERS} DESTINATION include/mgz-diff)
install(TARGETS mgz-diff mgz-diff-s
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
  
install(FILES ${MGZ_DIFF_OUTPUT_TEMP_DIR}/mgz-diff.pc DESTINATION lib/pkgconfig)
