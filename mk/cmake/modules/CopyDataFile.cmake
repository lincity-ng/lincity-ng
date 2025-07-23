## ---------------------------------------------------------------------- ##
 # mk/cmake/modules/CopyDataFile.cmake
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
