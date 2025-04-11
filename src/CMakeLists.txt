
include_directories(.)

configure_file(config.h.in config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(tinygettext)
add_subdirectory(gui)
add_subdirectory(lincity)
add_subdirectory(lincity-ng)
#add_subdirectory(TinyLandGen)
add_subdirectory(tools)
