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


if(NOT LINCITYNG_MACOS_BUNDLE)
  copy_data_file(${APPSTREAM_ID}.desktop ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_INSTALL_DATADIR}/applications data_files_out)
  copy_data_file(${APPSTREAM_ID}.metainfo.xml ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_INSTALL_DATADIR}/metainfo data_files_out)
  copy_data_file(lincity-ng.6 ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_INSTALL_MANDIR} data_files_out)
endif()

# app icon for Windows
if(WIN32)
  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${APPSTREAM_ID}.ico.rc "appicon ICON \"${CMAKE_CURRENT_SOURCE_DIR}/${APPSTREAM_ID}.ico\"")
  target_sources(lincity-ng PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/${APPSTREAM_ID}.ico.rc)
endif()

# app icon for macOS
if(LINCITYNG_MACOS_BUNDLE)
  set(MACOS_ICNS_FILE "${CMAKE_CURRENT_BINARY_DIR}/${APPSTREAM_ID}.icns")
  set(MACOS_ICONSET_DIR "${CMAKE_CURRENT_BINARY_DIR}/${APPSTREAM_ID}.iconset")
  file(MAKE_DIRECTORY "${MACOS_ICONSET_DIR}")
  foreach(iconSize IN ITEMS 16 32 128 256 512)
    math(EXPR iconSize2x "${iconSize} * 2")
    execute_process(
      COMMAND sips -z ${iconSize} ${iconSize} "${CMAKE_CURRENT_SOURCE_DIR}/${APPSTREAM_ID}_2x.png"
        --out "${MACOS_ICONSET_DIR}/icon_${iconSize}x${iconSize}.png"
      RESULT_VARIABLE sipsResult
      OUTPUT_QUIET
    )
    if(NOT sipsResult EQUAL 0)
      message(FATAL_ERROR "Failed to generate ${iconSize}x${iconSize} icon via sips")
    endif()
    execute_process(
      COMMAND sips -z ${iconSize2x} ${iconSize2x} "${CMAKE_CURRENT_SOURCE_DIR}/${APPSTREAM_ID}_2x.png"
        --out "${MACOS_ICONSET_DIR}/icon_${iconSize}x${iconSize}@2x.png"
      RESULT_VARIABLE sipsResult
      OUTPUT_QUIET
    )
    if(NOT sipsResult EQUAL 0)
      message(FATAL_ERROR "Failed to generate ${iconSize}x${iconSize}@2x icon via sips")
    endif()
  endforeach()
  execute_process(
    COMMAND iconutil -c icns "${MACOS_ICONSET_DIR}" -o "${MACOS_ICNS_FILE}"
    RESULT_VARIABLE iconutilResult
  )
  if(NOT iconutilResult EQUAL 0)
    message(FATAL_ERROR "Failed to generate .icns via iconutil")
  endif()
  # Would otherwise ship inside the bundle.
  file(REMOVE_RECURSE "${MACOS_ICONSET_DIR}")
  install(FILES "${MACOS_ICNS_FILE}"
    DESTINATION "${CMAKE_INSTALL_APPDATADIR}"
  )
endif()

if(NOT LINCITYNG_MACOS_BUNDLE)
  install(FILES ${APPSTREAM_ID}.png
    DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/128x128/apps"
  )
  install(FILES ${APPSTREAM_ID}_2x.png
    RENAME ${APPSTREAM_ID}.png
    DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps"
  )
endif()

add_custom_target(data-files.target ALL DEPENDS ${data_files_out})


file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/1328-to-2130.sh
  "#!/usr/bin/env bash\nzcat | sed '3,\$y/ /-/' | xsltproc ${CMAKE_INSTALL_FULL_APPDATADIR}/1328-to-2130.xsl - | gzip -9 -c")
install(FILES ${CMAKE_SOURCE_DIR}/contrib/ldsv-format-convert/1328-to-2130.xsl
  DESTINATION ${CMAKE_INSTALL_APPDATADIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/1328-to-2130.sh
  DESTINATION ${CMAKE_INSTALL_APPDATADIR})
