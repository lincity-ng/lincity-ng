set(guiDialogSourceFiles
  allgone.xml
  blacksmithup.xml
  bulldoze_monument_yn.xml
  bulldoze_river_yn.xml
  bulldoze_shanty_yn.xml
  coalmineup.xml
  coalpowerup.xml
  coal_survey_yn.xml
  cricketup.xml
  firestationup.xml
  fire.xml
  gamestats.xml
  healthup.xml
  hvindustryup.xml
  import-exportup.xml
  launch-evac.xml
  launch-fail.xml
  launch-gone.xml
  launch-good.xml
  launch_rocket_yn.xml
  ltindustryup.xml
  millup.xml
  mod_wind_up.xml
  nobull-tip.xml
  no-credit-parkland.xml
  no-credit-recycle.xml
  no-credit-rocket.xml
  no-credit-solar-power.xml
  no-credit-university.xml
  parkup.xml
  portdialog.xml
  potteryup.xml
  railwayup.xml
  recycleup.xml
  riverup.xml
  roadup.xml
  rocketup.xml
  schoolup.xml
  solarpowerup.xml
  sustain.xml
  tradedialog.xml
  universityup.xml
  warning.xml
  windmillup.xml
)

foreach(guiDialogSourceFile ${guiDialogSourceFiles})
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${guiDialogSourceFile}
    COMMAND LibXslt::xsltproc -o ${CMAKE_CURRENT_BINARY_DIR}/${guiDialogSourceFile} ${guiDialogSourceFile}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${guiDialogSourceFile}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Convert XML GUI dialogs/${guiDialogSourceFile}"
  )

  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${guiDialogSourceFile} DESTINATION ${CMAKE_INSTALL_APPDATADIR}/gui/dialogs/)
endforeach()

add_custom_target(guiDialogXml DEPENDS ${guiDialogSourceFiles})
add_dependencies(lincity-ng guiDialogXml)
