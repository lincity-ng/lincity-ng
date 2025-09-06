add_subdirectory(dialogs)

set(guiSourceFiles
  app.xml
  buttonpanel.xml
  creditslist.xml
  credits.xml
  dialog.xml
  helpwindow.xml
  loadgame.xml
  mainmenu.xml
  minimapswitchbuttons.xml
  minimap.xml
  mps.xml
  newgame.xml
  options.xml
  pbar2nd.xml
  pbar.xml
  savegame.xml
  speedpanel.xml
)

set(xslFiles identity.xsl)

foreach(guiSourceFile ${guiSourceFiles})
  if(ENABLE_NLS)
    set(XML_NLS enabled)
  else()
    set(XML_NLS disabled)
  endif()
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${guiSourceFile}
    COMMAND LibXslt::xsltproc -o ${CMAKE_CURRENT_BINARY_DIR}/${guiSourceFile} ${guiSourceFile}
      --stringparam NLS ${XML_NLS}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${guiSourceFile} ${xslFiles}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Converting ${guiSourceFile}"
  )

  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${guiSourceFile} DESTINATION ${CMAKE_INSTALL_APPDATADIR}/gui/)
endforeach()

add_custom_target(guiXml ALL DEPENDS ${guiSourceFiles} SOURCES ${guiSourceFiles})

include(TranslateTarget)
translate_target_xml(guiXml guiXml.pot ${CMAKE_CURRENT_SOURCE_DIR}/gui.its)
