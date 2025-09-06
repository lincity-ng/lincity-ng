## ---------------------------------------------------------------------- ##
 # mk/cmake/modules/FindLibXml++-5.0.cmake
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

find_package(PkgConfig)
if(PkgConfig_FOUND)
  pkg_check_modules(LibXml++-5.0 IMPORTED_TARGET libxml++-5.0)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibXml++-5.0
  REQUIRED_VARS LibXml++-5.0_FOUND
  VERSION_VAR LibXml++-5.0_VERSION
  HANDLE_VERSION_RANGE
)

if(LibXml++-5.0_FOUND)
  add_library(LibXml++-5.0::LibXml++-5.0 ALIAS PkgConfig::LibXml++-5.0)
endif()
