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

# miscellaneous files
copy_data_file(lincity-ng.desktop ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_INSTALL_DATADIR}/applications data_files_out)
copy_data_file(lincity-ng.png ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_INSTALL_DATADIR}/icons/hicolor/128x128/apps data_files_out)
copy_data_file(lincity-ng.6 ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_INSTALL_MANDIR} data_files_out)

add_custom_target(data-files.target ALL DEPENDS ${data_files_out})

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/1328-to-2130.sh
  "#!/usr/bin/env bash\nzcat | sed '3,\$y/ /-/' | xsltproc ${CMAKE_INSTALL_FULL_APPDATADIR}/1328-to-2130.xsl - | gzip -9 -c")
install(FILES ${CMAKE_SOURCE_DIR}/contrib/ldsv-format-convert/1328-to-2130.xsl
  DESTINATION ${CMAKE_INSTALL_APPDATADIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/1328-to-2130.sh
  DESTINATION ${CMAKE_INSTALL_APPDATADIR})
