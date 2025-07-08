
include_directories(.)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

configure_file(config.h.in config.h)

add_subdirectory(gui)
add_subdirectory(lincity)
add_subdirectory(lincity-ng)
