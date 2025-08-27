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

add_library(gettextutil INTERFACE gettextutil.hpp)
target_link_libraries(gettextutil INTERFACE gettext)
target_include_directories(gettextutil INTERFACE ..)

add_library(randutil STATIC randutil.cpp randutil.hpp)
target_include_directories(randutil INTERFACE ..)

add_library(ptrutil INTERFACE ptrutil.hpp)
target_include_directories(ptrutil INTERFACE ..)

include_directories(.)
