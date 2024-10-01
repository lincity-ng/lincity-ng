add_subdirectory(gui)


file(GLOB_RECURSE allDataFiles RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS
  fonts/*
  help/*
  images/*.png
  images/*.xml
  locale/*
  music/*
  opening/*.scn.gz
  sounds/*
  colour.pal
  lincityconfig.xml
  lincity-ng.desktop
  lincity-ng.png
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

# install the desktop file and icon
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lincity-ng.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lincity-ng.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/128x128/apps)
# install desktop file as a sym-link
# install(CODE "
#   if(\${CMAKE_INSTALL_PREFIX} EQUAL ${CMAKE_INSTALL_PREFIX})
#   message(STATUS \"installing desktop file: \${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/applications/lincity-ng.desktop\\nLinks to: ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_APPDATADIR}/lincity-ng.desktop\")
#   file(MAKE_DIRECTORY \${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/applications/)
#   execute_process(COMMAND \${CMAKE_COMMAND} -E create_symlink
#     ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_APPDATADIR}/lincity-ng.desktop
#     \${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/applications/lincity-ng.desktop
#   )
# ")


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
  COMMAND ${GETTEXT_XGETTEXT} --keyword='_:1' --keyword='N_:1' -o ${CMAKE_CURRENT_BINARY_DIR}/locale/messages.pot ${TRANSLATABLE_SOURCES}
  DEPENDS ${TRANSLATABLE_SOURCES}
  COMMENT "generating ${CMAKE_INSTALL_APPDATADIR}/locale/messages.pot"
)
add_custom_target(messages-pot.target DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/locale/messages.pot)
# add_dependencies(lincity-ng messages-pot.target)
# install(FILES ${CMAKE_CURRENT_BINARY_DIR}/locale/messages.pot DESTINATION ${CMAKE_INSTALL_APPDATADIR}/locale)

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
# add_dependencies(lincity-ng gui_messages-pot.target)
# install(FILES ${CMAKE_CURRENT_BINARY_DIR}/locale/gui/messages.pot DESTINATION ${CMAKE_INSTALL_APPDATADIR}/locale/gui)

add_custom_target(messages.pot DEPENDS messages-pot.target gui_messages-pot.target)


set(infoFiles
  COPYING
  COPYING-data.txt
  COPYING-fonts.txt
  README.md
  CHANGELOG.md
)
if(WIN32)
  set(infoFiles ${infoFiles} COPYING-dll.txt)
endif()
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
