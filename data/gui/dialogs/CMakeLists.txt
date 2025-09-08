set(guiDialogSourceFiles
  coal_survey_yn.xml
  gamestats.xml
  launch_rocket_yn.xml
  portdialog.xml
  tradedialog.xml
)

set(xslFiles dialogs.xsl)

foreach(guiDialogSourceFile ${guiDialogSourceFiles})
  if(ENABLE_NLS)
    set(XML_NLS enabled)
  else()
    set(XML_NLS disabled)
  endif()
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${guiDialogSourceFile}
    COMMAND LibXslt::xsltproc -o ${CMAKE_CURRENT_BINARY_DIR}/${guiDialogSourceFile} ${guiDialogSourceFile}
      --stringparam NLS ${XML_NLS}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${guiDialogSourceFile} ${xslFiles}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Converting dialogs/${guiDialogSourceFile}"
  )

  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${guiDialogSourceFile} DESTINATION ${CMAKE_INSTALL_APPDATADIR}/gui/dialogs/)
endforeach()

add_custom_target(guiDialogXml DEPENDS ${guiDialogSourceFiles})
add_dependencies(lincity-ng guiDialogXml)
