## ---------------------------------------------------------------------- ##
 # CMakeLists.txt
 # This file is part of Lincity-NG.
 #
 # Copyright (C) 2023      Edu Garcia
 # Copyright (C) 2023-2025 David Bears <dbear4q@gmail.com>
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

cmake_minimum_required(VERSION 3.21)

project(lincity-ng
  VERSION 2.14.1
  LANGUAGES CXX C
  DESCRIPTION "A city simulation game"
  HOMEPAGE_URL "https://github.com/lincity-ng/lincity-ng"
)
set(PROJECT_NAME_PRETTY "LinCity-NG")
set(APPSTREAM_ID "io.github.lincity_ng.lincity-ng")


### dependencies
list(APPEND CMAKE_MODULE_PATH
  ${CMAKE_SOURCE_DIR}/mk/cmake/modules
  ${CMAKE_SOURCE_DIR}/mk/cmake/modules/SDL2
)
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)
find_package(Threads)
find_package(ZLIB 1.0 REQUIRED)
find_package(SDL2 2.0.18 REQUIRED)
find_package(SDL2_image 2.0.0 REQUIRED)
find_package(SDL2_mixer 2.0.0 REQUIRED)
find_package(SDL2_ttf 2.0.12 REQUIRED)
find_package(LibXml2 REQUIRED) # requires >=2.6.11
find_package(fmt 9.0.0 REQUIRED)
find_package(LibXml++-5.0 REQUIRED)
find_package(Gettext)
find_package(Intl)
find_program(GETTEXT_XGETTEXT NAMES xgettext)
find_program(INCLUDE_WHAT_YOU_USE NAMES include-what-you-use)
find_package(Git)
# find_package(LibXslt NO_CMAKE_FIND_ROOT_PATH)
# The libxslt find module is broken when cross compiling because it looks in the
# target environment instead of the host. The workaround is to find only the
# xsltproc program.
if(NOT TARGET LibXslt::xsltproc)
	find_program(LIBXSLT_XSLTPROC_EXECUTABLE NAMES xsltproc REQUIRED)
	if(LIBXSLT_XSLTPROC_EXECUTABLE)
		add_executable(LibXslt::xsltproc IMPORTED)
		set_target_properties(LibXslt::xsltproc PROPERTIES IMPORTED_LOCATION "${LIBXSLT_XSLTPROC_EXECUTABLE}")
	endif()
endif()


### build configurations
set(BUILD_TYPES Release MinSizeRel RelWithDebInfo BetaTest Debug DebugOpt)
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${BUILD_TYPES})
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
	message(STATUS "No build type specified, defaulting to ${CMAKE_BUILD_TYPE}.")
endif()
string(TOUPPER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_UPPER)
set(CMAKE_CXX_FLAGS_RELEASE        "-O3 -w -DNDEBUG")
set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -w -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g -DNDEBUG")
set(CMAKE_CXX_FLAGS_BETATEST       "-O3 -g -w")
set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUGOPT       "-O2 -g -DDEBUG")
foreach(BType ${BUILD_TYPES})
	string(TOUPPER ${BType} BTYPE)
	set(CMAKE_CXX_FLAGS_${BTYPE} ${CMAKE_CXX_FLAGS_${BTYPE}}
		CACHE STRING "Flags used by the CXX compiler for ${BTYPE} builds.")
	set(CMAKE_C_FLAGS_${BCONFIG} ${CMAKE_CXX_FLAGS_${BTYPE}}
		CACHE STRING "Flags used by the C compiler for ${BTYPE} builds.")
endforeach()


### options
set(ENABLE_NLS "YES" CACHE BOOL "Enable native language support")
if(ENABLE_NLS AND NOT (Intl_FOUND AND Gettext_FOUND))
  message(WARNING "Native language support requires gettext and libintl, but"
    " one or both are missing. To suppress this warning, explicitly disable"
    " native language support with `-DENABLE_NLS=NO`.")
endif()
set(LINCITYNG_RELOCATABLE "NO" CACHE BOOL "Build a relocatable binary")


### compilation options
set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options("-fmacro-prefix-map=${CMAKE_SOURCE_DIR}=.")
if(UNIX)
	add_compile_definitions("UNIX")
endif()
if(WIN32)
	add_compile_definitions("WIN32")
endif()
if(APPLE)
	add_compile_definitions("APPLE")
endif()

include(CheckCSourceCompiles)
set(CMAKE_REQUIRED_LIBRARIES ${LibIntl_LIBRARIES})
check_c_source_compiles("\
	extern int _nl_msg_cat_cntr; \
	int main() { \
		++_nl_msg_cat_cntr; \
		return 0; \
	} \
	"
	HAVE_NL_MSG_CAT_CNTR
)


### include what you use
if(NOT CMAKE_CROSSCOMPILING AND INCLUDE_WHAT_YOU_USE AND
	NOT CMAKE_CXX_FLAGS_${BUILD_TYPE_UPPER} MATCHES "(^| )-w( |$)"
)
	set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${INCLUDE_WHAT_YOU_USE}
		-w -Xiwyu --mapping_file=${CMAKE_SOURCE_DIR}/mk/iwyu.map)
	set(CMAKE_C_INCLUDE_WHAT_YOU_USE ${CMAKE_CXX_INCLUDE_WHAT_YOU_USE})
endif()


### compute LinCity-NG version
set(LINCITYNG_VERSION_SUFFIX "" CACHE STRING
	"Custom version suffix to use; mostly useful for downstream packaging."
)
include(GetGitRevisionDescription)
if(DEFINED LINCITYNG_VERSION_SUFFIX AND NOT (LINCITYNG_VERSION_SUFFIX STREQUAL ""))
	set(FULL_PROJECT_VERSION "${PROJECT_VERSION}-${LINCITYNG_VERSION_SUFFIX}")
else()
	git_describe_working_tree(FULL_PROJECT_VERSION --match lincity-ng-*)
	if(FULL_PROJECT_VERSION)
		string(SUBSTRING ${FULL_PROJECT_VERSION} 11 -1 FULL_PROJECT_VERSION)
	else()
		set(FULL_PROJECT_VERSION "${PROJECT_VERSION}-unknown")
	endif()
endif()
set(VERSION_RELEASE "")
set(VERSION_MINSIZEREL "")
set(VERSION_RELWITHDEBINFO "debinfo")
set(VERSION_BETATEST "beta")
set(VERSION_DEBUG "debug")
set(VERSION_DEBUGOPT "debug")
if(VERSION_${BUILD_TYPE_UPPER})
	set(FULL_PROJECT_VERSION ${FULL_PROJECT_VERSION}-${VERSION_${BUILD_TYPE_UPPER}})
elseif(NOT DEFINED VERSION_${BUILD_TYPE_UPPER})
	message(WARNING "unrecognized build type")
endif()
message(STATUS "LinCity-NG version: ${FULL_PROJECT_VERSION}")


### compute installation directories
include(GNUInstallDirs)
set(CMAKE_INSTALL_APPDATADIR ${CMAKE_INSTALL_DATADIR}/${CMAKE_PROJECT_NAME})
set(CMAKE_INSTALL_FULL_APPDATADIR ${CMAKE_INSTALL_FULL_DATADIR}/${CMAKE_PROJECT_NAME})
set(CMAKE_BINARY_BINDIR ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
set(CMAKE_BINARY_DATADIR ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_DATADIR})
set(CMAKE_BINARY_APPDATADIR ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_APPDATADIR})


### configure subdirectories
add_subdirectory(src)
add_subdirectory(data ${CMAKE_INSTALL_APPDATADIR})
add_subdirectory(doc ${CMAKE_INSTALL_DOCDIR})
add_subdirectory(lib)


### runtime dependency installation for Windows
set_target_properties(lincity-ng PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_BINDIR})
install(TARGETS lincity-ng
	RUNTIME_DEPENDENCY_SET dll_dependencies
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
if(WIN32)
	# from https://gitlab.kitware.com/cmake/cmake/-/issues/20753#note_870046
	if(MINGW AND CMAKE_CROSSCOMPILING)
		# workaround for crosscompiling on linux/mingw for windows
		install(CODE [[
			set(CMAKE_GET_RUNTIME_DEPENDENCIES_PLATFORM "windows+pe")
			set(CMAKE_GET_RUNTIME_DEPENDENCIES_TOOL "objdump")
			set(CMAKE_GET_RUNTIME_DEPENDENCIES_COMMAND "${CMAKE_OBJDUMP}")
			# file(WRITE "objdump_unix2dos.sh" "${CMAKE_OBJDUMP} $@ | unix2dos")
			# file(CHMOD "objdump_unix2dos.sh" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE)
		]])
	endif()

	install(RUNTIME_DEPENDENCY_SET dll_dependencies
		DIRECTORIES ${CMAKE_FIND_ROOT_PATH}/bin  # dll's from mingw
		UNRESOLVED_DEPENDENCIES_VAR DUMMY  # hacky!
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)
endif()
# if (WIN32)
# 	install(FILES $<TARGET_PDB_FILE:lincity-ng> DESTINATION . OPTIONAL)
# endif()


### packaging
set(CPACK_GENERATOR TXZ)
if(WIN32)
	set(CPACK_GENERATOR ZIP)
endif()
set(CPACK_SOURCE_GENERATOR ${CPACK_GENERATOR})
set(CPACK_PACKAGE_ICON data/io.github.lincity_ng.lincity-ng.png)
set(CPACK_PACKAGE_CHECKSUM SHA256)
set(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README.md)
set(CPACK_SOURCE_IGNORE_FILES ${CMAKE_BINARY_DIR}/ ${CMAKE_SOURCE_DIR}/\\\\.git/ ${CMAKE_SOURCE_DIR}/build.*/)
set(CPACK_PACKAGE_VERSION ${FULL_PROJECT_VERSION})
if(CMAKE_CXX_FLAGS_${BUILD_TYPE_UPPER} MATCHES "(^| )-g( |$)")
	set(CPACK_STRIP_FILES FALSE)
else()
	set(CPACK_STRIP_FILES TRUE)
endif()
configure_file(mk/cmake/LCCPackConfig.cmake.in LCCPackConfig.cmake @ONLY)
set(CPACK_PROJECT_CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/LCCPackConfig.cmake")
include(CPack)
