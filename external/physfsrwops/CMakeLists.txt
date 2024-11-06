add_library(physfsrwops STATIC
  physfsrwops.c physfsrwops.h
)

target_include_directories(physfsrwops PUBLIC .)
target_link_libraries(physfsrwops
  PUBLIC
  $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
  ${PHYSFS_LIBRARY}
)
set_target_properties(physfsrwops PROPERTIES C_INCLUDE_WHAT_YOU_USE "")
