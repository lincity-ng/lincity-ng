add_subdirectory(dialogs)

set(guiSourceFiles
  app.xml
  buttonpanel.xml
  creditslist.xml
  credits.xml
  helpwindow.xml
  loadgame.xml
  mainmenu.xml
  messagearea.xml
  minimapswitchbuttons.xml
  minimap.xml
  mps.xml
  newgame.xml
  options.xml
  pbar2nd.xml
  pbar.xml
  piemenu.xml
  savegame.xml
  speedpanel.xml
  too-old.xml
)

foreach(guiSourceFile ${guiSourceFiles})
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${guiSourceFile}
    COMMAND LibXslt::xsltproc -o ${CMAKE_CURRENT_BINARY_DIR}/${guiSourceFile} ${guiSourceFile}
    DEPENDS ${guiSourceFile}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Convert XML GUI ${guiSourceFile}"
  )
  target_sources(lincity-ng PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/${guiSourceFile})
endforeach()
