add_subdirectory(gui)


file(GLOB_RECURSE allDataFiles RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS
  fonts/*
  help/*
  images/*
  locale/*
  music/*
  opening/*.scn
  sounds/*
  colour.pal
  lincityconfig.xml
)

foreach(dataFile ${allDataFiles})
  add_custom_command(
    OUTPUT ${dataFile}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${dataFile} ${dataFile}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${dataFile}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "copying ${dataFile} to build directory"
  )
endforeach()
add_custom_target(allDataFiles.target DEPENDS ${allDataFiles})
add_dependencies(lincity-ng allDataFiles.target)


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
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/locale)
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/locale/messages.pot
  COMMAND xgettext --keyword='_:1' --keyword='N_:1' -o ${CMAKE_CURRENT_BINARY_DIR}/locale/messages.pot ${TRANSLATABLE_SOURCES}
  DEPENDS ${TRANSLATABLE_SOURCES}
  COMMENT "generating ${CMAKE_INSTALL_APPDATADIR}/locale/messages.pot"
)
add_custom_target(messages-pot.target DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/locale/messages.pot)
add_dependencies(lincity-ng messages-pot.target)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/locale/messages.pot DESTINATION ${CMAKE_INSTALL_APPDATADIR}/locale)

file(GLOB_RECURSE ALL_GUI_XML RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/gui/*.xml
)
list(TRANSFORM ALL_GUI_XML PREPEND ${CMAKE_CURRENT_BINARY_DIR}/)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/locale/gui)
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/locale/gui/messages.pot
  COMMAND xmlgettext ${CMAKE_CURRENT_BINARY_DIR}/locale/gui/messages.pot ${ALL_GUI_XML}
  DEPENDS ${ALL_GUI_XML} guiXml guiDialogXml
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "generating ${CMAKE_INSTALL_APPDATADIR}/locale/gui/messages.pot"
)
add_custom_target(gui_messages-pot.target DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/locale/gui/messages.pot)
add_dependencies(lincity-ng gui_messages-pot.target)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/locale/gui/messages.pot DESTINATION ${CMAKE_INSTALL_APPDATADIR}/locale/gui)


install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/fonts DESTINATION ${CMAKE_INSTALL_APPDATADIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/help DESTINATION ${CMAKE_INSTALL_APPDATADIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/images DESTINATION ${CMAKE_INSTALL_APPDATADIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/locale DESTINATION ${CMAKE_INSTALL_APPDATADIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/music DESTINATION ${CMAKE_INSTALL_APPDATADIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/opening DESTINATION ${CMAKE_INSTALL_APPDATADIR} FILES_MATCHING PATTERN "*.scn")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/sounds DESTINATION ${CMAKE_INSTALL_APPDATADIR})

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/colour.pal DESTINATION ${CMAKE_INSTALL_APPDATADIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/lincityconfig.xml DESTINATION ${CMAKE_INSTALL_APPDATADIR})
