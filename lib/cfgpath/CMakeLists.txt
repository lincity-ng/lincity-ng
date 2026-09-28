add_library(cfgpath INTERFACE
  cfgpath.h
)

target_include_directories(cfgpath INTERFACE SYSTEM .)
set_target_properties(cfgpath PROPERTIES
  LINKER_LANGUAGE C
  C_INCLUDE_WHAT_YOU_USE ""
)

# APPLE: cfgpath.h's macOS path lookup uses deprecated Carbon symbols.
if(APPLE)
  target_link_libraries(cfgpath INTERFACE "-framework CoreServices")
endif()
