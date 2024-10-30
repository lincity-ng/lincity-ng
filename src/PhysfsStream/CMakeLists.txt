add_library(physfsstream STATIC
  PhysfsStream.cpp
  PhysfsStream.hpp

  PhysfsError.hpp
  PhysfsError.cpp
)
add_library(LinCity::physfsstream ALIAS physfsstream)

target_link_libraries(physfsstream
  PUBLIC
  $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
  ${PHYSFS_LIBRARY}
)

target_include_directories(physfsstream
  PUBLIC
  ${PHYSFS_INCLUDE_DIR}
)

add_library(physfsrwops STATIC)
include(FetchContent)
set(physfsrwops.c_SHA256 ffebf2292e426d1d6c5874b5264e837244dbe7e24b792419a518cf09c921f8bd)
set(physfsrwops.h_SHA256 8a40e265c9506b3bf27148db3687104824f13ec167212f913aa2ecd3e57bb3bc)
foreach(physfsrwops_file physfsrwops.c;physfsrwops.h)
  FetchContent_Declare(${physfsrwops_file}
    URL https://github.com/icculus/physfs/raw/74c30545031ca8cdb69b2f1ec173e77d79078093/extras/${physfsrwops_file}
    URL_HASH SHA256=${${physfsrwops_file}_SHA256}
    DOWNLOAD_NO_EXTRACT TRUE
  )
  FetchContent_MakeAvailable(${physfsrwops_file})
  target_sources(physfsrwops PRIVATE ${${physfsrwops_file}_SOURCE_DIR}/${physfsrwops_file})
endforeach()
target_include_directories(physfsrwops PUBLIC ${physfsrwops.h_SOURCE_DIR})
target_link_libraries(physfsrwops
  PUBLIC
  $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
  ${PHYSFS_LIBRARY}
)
set_target_properties(physfsrwops PROPERTIES C_INCLUDE_WHAT_YOU_USE "")

include_directories(.)
