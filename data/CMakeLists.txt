add_subdirectory(gui)
add_subdirectory(locale)

# set_property(GLOBAL PROPERTY TARGET_MESSAGES OFF)

include(CopyDataFile)

# data files
file(GLOB_RECURSE allDataFiles RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS
  fonts/*
  help/*
  images/*.png
  images/*.xml
  music/*
  opening/*.scn.gz
  sounds/*
  lincityconfig.xml
)
foreach(dataFile ${allDataFiles})
  copy_data_file(${dataFile} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_INSTALL_APPDATADIR} data_files_out)
endforeach()


copy_data_file(${APPSTREAM_ID}.desktop ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_INSTALL_DATADIR}/applications data_files_out)
copy_data_file(${APPSTREAM_ID}.metainfo.xml ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_INSTALL_DATADIR}/metainfo data_files_out)
copy_data_file(lincity-ng.6 ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_INSTALL_MANDIR} data_files_out)

# app icon for Windows
if(WIN32)
  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${APPSTREAM_ID}.ico.rc "appicon ICON \"${CMAKE_CURRENT_SOURCE_DIR}/${APPSTREAM_ID}.ico\"")
  target_sources(lincity-ng PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/${APPSTREAM_ID}.ico.rc)
endif()

install(FILES ${APPSTREAM_ID}.png
  DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/128x128/apps"
)
install(FILES ${APPSTREAM_ID}_2x.png
  RENAME ${APPSTREAM_ID}.png
  DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps"
)

add_custom_target(data-files.target ALL DEPENDS ${data_files_out})


file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/1328-to-2130.sh
  "#!/usr/bin/env bash\nzcat | sed '3,\$y/ /-/' | xsltproc ${CMAKE_INSTALL_FULL_APPDATADIR}/1328-to-2130.xsl - | gzip -9 -c")
install(FILES ${CMAKE_SOURCE_DIR}/contrib/ldsv-format-convert/1328-to-2130.xsl
  DESTINATION ${CMAKE_INSTALL_APPDATADIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/1328-to-2130.sh
  DESTINATION ${CMAKE_INSTALL_APPDATADIR})
