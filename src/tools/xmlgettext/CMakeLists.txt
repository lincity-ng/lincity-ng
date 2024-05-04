
add_executable(xmlgettext
  main.cpp
  XmlReader.cpp
  XmlReader.hpp
)

set_target_properties(xmlgettext PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tools)

target_link_libraries(xmlgettext
  PRIVATE
  LibXml2::LibXml2
)
target_include_directories(xmlgettext
  PRIVATE
  ${LIBXML2_INCLUDE_DIR}
)
