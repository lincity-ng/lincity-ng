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
  lincity-ng.desktop
)

foreach(dataFile ${allDataFiles})
  add_custom_command(
    OUTPUT ${dataFile}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${dataFile} ${dataFile}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${dataFile}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "copying ${dataFile} to build directory"
  )
  cmake_path(GET dataFile PARENT_PATH dataFileDst)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${dataFile} DESTINATION ${CMAKE_INSTALL_APPDATADIR}/${dataFileDst})
endforeach()
add_custom_target(allDataFiles.target DEPENDS ${allDataFiles})
add_dependencies(lincity-ng allDataFiles.target)
install(CODE "
  message(STATUS \"installing desktop file: ${CMAKE_INSTALL_FULL_DATADIR}/applications/lincity-ng.desktop\")
  file(MAKE_DIRECTORY ${CMAKE_INSTALL_FULL_DATADIR}/applications/)
  execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_INSTALL_FULL_APPDATADIR}/lincity-ng.desktop
    ${CMAKE_INSTALL_FULL_DATADIR}/applications/lincity-ng.desktop
  )
")


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


set(infoFiles
  COPYING
  COPYING-data.txt
  COPYING-fonts.txt
  README.md
  RELNOTES
  TODO
)
foreach(infoFile ${infoFiles})
  add_custom_command(
    OUTPUT ${infoFile}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/${infoFile} ${infoFile}
    DEPENDS ${CMAKE_SOURCE_DIR}/${infoFile}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "copying ${infoFile} to build directory"
  )
  cmake_path(GET infoFile PARENT_PATH infoFileDst)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${infoFile} DESTINATION ${CMAKE_INSTALL_APPDATADIR}/${infoFileDst})
endforeach()
add_custom_target(infoFiles.target DEPENDS ${infoFiles})
add_dependencies(lincity-ng infoFiles.target)
