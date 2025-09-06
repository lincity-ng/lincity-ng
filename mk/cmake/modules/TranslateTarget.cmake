## ---------------------------------------------------------------------- ##
 # mk/cmake/modules/TranslateTarget.cmake
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

function(translate_target target)
  get_target_property(TARGET_SOURCES_REL ${target} SOURCES)
  foreach(SOURCE_PATH ${TARGET_SOURCES_REL})
    cmake_path(ABSOLUTE_PATH SOURCE_PATH
      BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    list(APPEND TARGET_SOURCES ${SOURCE_PATH})
  endforeach()
  add_custom_command(
    OUTPUT ${target}.pot
    COMMAND ${GETTEXT_XGETTEXT}
      --keyword='_:1' --keyword='N_:1' -o ${target}.pot ${TARGET_SOURCES}
    DEPENDS ${TARGET_SOURCES}
    COMMENT "generating ${target}.pot"
  )
  add_custom_target(${target}.pot.target
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${target}.pot
  )
endfunction(translate_target)

function(translate_target_xml target its)
  get_target_property(TARGET_SOURCES_REL ${target} SOURCES)
  foreach(SOURCE_PATH ${TARGET_SOURCES_REL})
    cmake_path(ABSOLUTE_PATH SOURCE_PATH
      BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    list(APPEND TARGET_SOURCES ${SOURCE_PATH})
  endforeach()
  add_custom_command(
    OUTPUT ${target}.pot
    COMMAND ${GETTEXT_XGETTEXT} --its=${its} -o ${target}.pot ${TARGET_SOURCES}
    DEPENDS ${TARGET_SOURCES} ${its}
    COMMENT "generating ${target}.pot"
  )
  add_custom_target(${target}.pot.target
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${target}.pot
  )
endfunction(translate_target_xml)
