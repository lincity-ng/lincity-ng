## ---------------------------------------------------------------------- ##
 # lib/gettext/CMakeLists.txt
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

add_library(gettext INTERFACE
  gettext.h
)

target_include_directories(gettext INTERFACE SYSTEM .)
set_target_properties(gettext PROPERTIES
  LINKER_LANGUAGE C
  C_INCLUDE_WHAT_YOU_USE ""
)

if(ENABLE_NLS AND Intl_FOUND)
  target_link_libraries(gettext INTERFACE Intl::Intl)
  target_compile_definitions(gettext INTERFACE ENABLE_NLS=1)
endif()
