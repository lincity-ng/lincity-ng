add_custom_target(messages.pot DEPENDS messages-pot.target gui_messages-pot.target)

file(RELATIVE_PATH CURRENT_BINARY_DIR_REL
  ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})


file(GLOB_RECURSE TRANSLATABLE_SOURCES CONFIGURE_DEPENDS
  ${CMAKE_SOURCE_DIR}/src/gui/*.cpp
  ${CMAKE_SOURCE_DIR}/src/gui/*.hpp
  ${CMAKE_SOURCE_DIR}/src/gui/*.h
  ${CMAKE_SOURCE_DIR}/src/lincity/*.cpp
  ${CMAKE_SOURCE_DIR}/src/lincity/*.hpp
  ${CMAKE_SOURCE_DIR}/src/lincity/*.h
  ${CMAKE_SOURCE_DIR}/src/lincity-ng/*.cpp
  ${CMAKE_SOURCE_DIR}/src/lincity-ng/*.hpp
  ${CMAKE_SOURCE_DIR}/src/lincity-ng/*.h
)
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/messages.pot
  COMMAND ${GETTEXT_XGETTEXT} --keyword='_:1' --keyword='N_:1' -o ${CMAKE_CURRENT_BINARY_DIR}/messages.pot ${TRANSLATABLE_SOURCES}
  DEPENDS ${TRANSLATABLE_SOURCES}
  COMMENT "generating ${CURRENT_BINARY_DIR_REL}/messages.pot"
)
add_custom_target(messages-pot.target
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/messages.pot)


file(GLOB_RECURSE TRANSLATABLE_GUI RELATIVE ${CMAKE_SOURCE_DIR}/data/gui CONFIGURE_DEPENDS
  ${CMAKE_SOURCE_DIR}/data/gui/*.xml
)
list(TRANSFORM TRANSLATABLE_GUI PREPEND ${CMAKE_BINARY_APPDATADIR}/gui/)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/gui)
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/gui/messages.pot
  COMMAND xmlgettext ${CMAKE_CURRENT_BINARY_DIR}/gui/messages.pot ${TRANSLATABLE_GUI}
  DEPENDS ${TRANSLATABLE_GUI} guiXml guiDialogXml
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "generating ${CURRENT_BINARY_DIR_REL}/gui/messages.pot"
)
add_custom_target(gui_messages-pot.target DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/gui/messages.pot)
