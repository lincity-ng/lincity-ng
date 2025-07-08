add_subdirectory(gui)
add_subdirectory(locale)

# set_property(GLOBAL PROPERTY TARGET_MESSAGES OFF)

function(copy_data_file data_file src_loc dst_loc)
  set(build_path ${CMAKE_BINARY_DIR}/${dst_loc}/${data_file})
  set(install_path ${dst_loc}/${data_file})
  add_custom_command(
    OUTPUT ${build_path}
    COMMAND ${CMAKE_COMMAND} -E copy ${src_loc}/${data_file} ${build_path}
    DEPENDS ${src_loc}/${data_file}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "copying ${data_file} to build directory"
  )
  if(ARGC GREATER_EQUAL 4)
    list(APPEND ${ARGV3} ${build_path})
    set(${ARGV3} ${${ARGV3}} PARENT_SCOPE)
  else()
    string(REPLACE "/" "_" tmp_target ${build_path})
    add_custom_target(${tmp_target} ALL DEPENDS ${build_path})
  endif()
  cmake_path(GET install_path PARENT_PATH install_path_parent)
  install(FILES ${build_path} DESTINATION ${install_path_parent})
endfunction(copy_data_file)

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

set(infoFiles
  CHANGELOG.md
  CONTRIBUTING.md
  COPYING
  COPYING-data.txt
  COPYING-fonts.txt
  README.md
)
if(WIN32)
  set(infoFiles ${infoFiles} COPYING-dll.txt)
endif()
foreach(infoFile ${infoFiles})
  copy_data_file(${infoFile} ${CMAKE_SOURCE_DIR} ${CMAKE_INSTALL_DOCDIR} data_files_out)
endforeach()

copy_data_file(lincity-ng.desktop ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_INSTALL_DATADIR}/applications data_files_out)
copy_data_file(lincity-ng.png ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_INSTALL_DATADIR}/icons/hicolor/128x128/apps data_files_out)

add_custom_target(data-files.target ALL DEPENDS ${data_files_out})

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/1328-to-2130.sh
  "#!/usr/bin/env bash\nzcat | sed '3,\$y/ /-/' | xsltproc ${CMAKE_INSTALL_FULL_APPDATADIR}/1328-to-2130.xsl - | gzip -9 -c")
install(FILES ${CMAKE_SOURCE_DIR}/contrib/ldsv-format-convert/1328-to-2130.xsl
  DESTINATION ${CMAKE_INSTALL_APPDATADIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/1328-to-2130.sh
  DESTINATION ${CMAKE_INSTALL_APPDATADIR})
