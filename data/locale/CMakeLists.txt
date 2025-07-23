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


set(LINGUAS
  ca cs da de el es fr gd gl hu ja nl pl pt_BR ru sv tr zh_CN
)

if(DEFINED ENV{LINGUAS})
  set(LINGUAS $ENV{LINGUAS})
elseif(DEFINED CACHE{LINGUAS})
  set(LINGUAS $CACHE{LINGUAS})
endif()


### .POT generation
set(POT_FILES
  ${CMAKE_BINARY_DIR}/src/lincity-ng/lincity-ng.pot
  ${CMAKE_BINARY_DIR}/src/lincity/lincity_lib.pot
  ${CMAKE_BINARY_DIR}/src/lincity/modules/lincity_lib_modules.pot
  ${CMAKE_BINARY_DIR}/share/lincity-ng/gui/guiXml.pot
)
foreach(pot ${POT_FILES})
  cmake_path(GET pot FILENAME potname)
  list(APPEND POT_TARGETS ${potname}.target)
endforeach()
add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/messages.pot
  COMMAND ${GETTEXT_XGETTEXT} -o ${CMAKE_BINARY_DIR}/messages.pot ${POT_FILES}
  DEPENDS ${POT_TARGETS}
  COMMENT "merging messages.pot"
)
add_custom_target(generate_pot DEPENDS ${CMAKE_BINARY_DIR}/messages.pot)


### .MO generation
if(GETTEXT_FOUND AND ENABLE_NLS)
  foreach(lang ${LINGUAS})
    set(BUILD_DEST
      ${CMAKE_CURRENT_BINARY_DIR}/${lang}/LC_MESSAGES/${CMAKE_PROJECT_NAME}.mo
    )
    gettext_process_po_files(${lang} PO_FILES ${lang}.po)
    add_custom_command(
      OUTPUT ${BUILD_DEST}
      COMMAND ${CMAKE_COMMAND} -E copy ${lang}.gmo ${BUILD_DEST}
      DEPENDS ${lang}.gmo
      COMMENT "copying ${lang} locale to build destination"
    )
    list(APPEND built_locales ${BUILD_DEST})
    install(FILES ${BUILD_DEST}
      DESTINATION ${CMAKE_INSTALL_APPDATADIR}/locale/${lang}/LC_MESSAGES/
    )
  endforeach()
  add_custom_target(locales ALL DEPENDS ${built_locales})
endif()
