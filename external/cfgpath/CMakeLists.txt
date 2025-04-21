add_library(cfgpath INTERFACE
  cfgpath.h
)

target_include_directories(cfgpath INTERFACE .)
set_target_properties(cfgpath PROPERTIES
  LINKER_LANGUAGE C
  C_INCLUDE_WHAT_YOU_USE ""
)
