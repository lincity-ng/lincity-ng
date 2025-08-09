add_library(xmlutil STATIC
  xmlutil.cpp
  xmlutil.hpp
)

target_link_libraries(xmlutil
  PUBLIC
  LibXml++-5.0::LibXml++-5.0
  PRIVATE
	fmt::fmt
)

target_include_directories(xmlutil INTERFACE ..)

include_directories(.)
