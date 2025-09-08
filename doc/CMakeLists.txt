## ---------------------------------------------------------------------- ##
 # doc/CMakeLists.txt
 # This file is part of Lincity-NG.
 #
 # Copyright (C) 2025      David Bears <dbear4q@gmail.com>
 #
 # This program is free software; you can redistribute it and/or modify
 # it under the terms of the GNU General Public License as published by
 # the Free Software Foundation; either version 2 of the License, or
 # (at your option) any later version.
 #
 # This program is distributed in the hope that it will be useful,
 # but WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 # GNU General Public License for more details.
 #
 # You should have received a copy of the GNU General Public License along
 # with this program; if not, write to the Free Software Foundation, Inc.,
 # 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
## ---------------------------------------------------------------------- ##

include(CopyDataFile)

set(docFiles
  doc/CONFIGURE_OPTIONS.md
  doc/DEPENDENCIES.md
  doc/LINCITY-NG_CONFIG.md

  CHANGELOG.md
  CONTRIBUTING.md
  COPYING.txt
  COPYING-data.txt
  COPYING-fonts.txt
  README.md
)
if(WIN32)
  list(APPEND docFiles COPYING-dll.txt)
endif()

foreach(docFile ${docFiles})
  cmake_path(GET docFile FILENAME docName)
  cmake_path(GET docFile PARENT_PATH docPath)
  copy_data_file(
    ${docName}
    ${CMAKE_SOURCE_DIR}/${docPath}
    ${CMAKE_INSTALL_DOCDIR}
    doc_files_list
  )
endforeach()

add_custom_target(doc-files.target ALL DEPENDS ${doc_files_list})
