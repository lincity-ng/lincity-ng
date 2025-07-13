cmake_minimum_required(VERSION 3.21)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(X_VCPKG_APPLOCAL_DEPS_INSTALL ON)

project(lincity-ng
	VERSION 2.14.0
	LANGUAGES CXX C
	DESCRIPTION "A city simulation game"
	HOMEPAGE_URL "https://github.com/lincity-ng/lincity-ng"
)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH
	${CMAKE_SOURCE_DIR}/mk/cmake/modules
	${CMAKE_SOURCE_DIR}/mk/cmake/modules/SDL2
)

include(CheckIncludeFiles)
include(GNUInstallDirs)
include(GetGitRevisionDescription)

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


# find dependencies
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)
find_package(Threads)
find_package(ZLIB 1.0 REQUIRED)
find_package(SDL2 2.0.18 REQUIRED)
find_package(SDL2_image 2.0.0 REQUIRED)
find_package(SDL2_mixer 2.0.0 REQUIRED)
find_package(SDL2_ttf 2.0.12 REQUIRED)
find_package(LibXml2 REQUIRED) # requires >=2.6.11
find_package(fmt 9.0.0 REQUIRED)
# seems libxml++ doesn't have a find module, but it provides a pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(libXMLPlusPlus REQUIRED IMPORTED_TARGET libxml++-5.0)

# find_package(LibXslt NO_CMAKE_FIND_ROOT_PATH)
# The libxslt find module is broken when cross compiling because it looks in the
# target environment instead of the host. The workaround is to find only the
# xsltproc program.
if(NOT TARGET LibXslt::xsltproc)
	if(NOT DEFINED LIBXSLT_XSLTPROC_EXECUTABLE)
		find_program(LIBXSLT_XSLTPROC_EXECUTABLE NAMES xsltproc REQUIRED)
	endif()
	if(DEFINED LIBXSLT_XSLTPROC_EXECUTABLE)
		add_executable(LibXslt::xsltproc IMPORTED)
		set_target_properties(LibXslt::xsltproc PROPERTIES IMPORTED_LOCATION "${LIBXSLT_XSLTPROC_EXECUTABLE}")
	endif()
endif()

find_program(GETTEXT_XGETTEXT NAMES xgettext)
find_program(INCLUDE_WHAT_YOU_USE NAMES include-what-you-use)


### compute the LinCity-NG version
git_describe_working_tree(FULL_PROJECT_VERSION --match lincity-ng-*)
if(FULL_PROJECT_VERSION)
	string(SUBSTRING ${FULL_PROJECT_VERSION} 11 -1 FULL_PROJECT_VERSION)
else()
	set(FULL_PROJECT_VERSION ${PROJECT_VERSION}-unknown)
endif()
set(BUILD_TYPE_RELEASE "")
set(BUILD_TYPE_MINSIZEREL "")
set(BUILD_TYPE_RELWITHDEBINFO "debinfo")
set(BUILD_TYPE_BETATEST "beta")
set(BUILD_TYPE_DEBUG "debug")
set(BUILD_TYPE_DEBUGOPT "debug")
if(BUILD_TYPE_${BUILD_TYPE_UPPER})
	set(FULL_PROJECT_VERSION ${FULL_PROJECT_VERSION}-${BUILD_TYPE_${BUILD_TYPE_UPPER}})
endif()


set(CMAKE_INSTALL_APPDATADIR ${CMAKE_INSTALL_DATADIR}/${CMAKE_PROJECT_NAME})
set(CMAKE_INSTALL_FULL_APPDATADIR ${CMAKE_INSTALL_FULL_DATADIR}/${CMAKE_PROJECT_NAME})
set(CMAKE_BINARY_BINDIR ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
set(CMAKE_BINARY_DATADIR ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_DATADIR})
set(CMAKE_BINARY_APPDATADIR ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_APPDATADIR})

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

if(NOT CMAKE_CROSSCOMPILING AND INCLUDE_WHAT_YOU_USE AND
	NOT CMAKE_CXX_FLAGS_${BUILD_TYPE_UPPER} MATCHES "(^| )-w( |$)"
)
	set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${INCLUDE_WHAT_YOU_USE}
		-w -Xiwyu --mapping_file=${CMAKE_SOURCE_DIR}/mk/iwyu.map)
	set(CMAKE_C_INCLUDE_WHAT_YOU_USE ${CMAKE_CXX_INCLUDE_WHAT_YOU_USE})
endif()

add_subdirectory(src)
add_subdirectory(data ${CMAKE_INSTALL_APPDATADIR})
add_subdirectory(external)

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
include(CPack)
