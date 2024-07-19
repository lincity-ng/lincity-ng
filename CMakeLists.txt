cmake_minimum_required(VERSION 3.21)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(X_VCPKG_APPLOCAL_DEPS_INSTALL ON)

project(lincity-ng
	VERSION 2.11.1
	LANGUAGES CXX C
	DESCRIPTION "A city simulation game"
	HOMEPAGE_URL "https://github.com/lincity-ng/lincity-ng"
)

list(APPEND CMAKE_MODULE_PATH
	${CMAKE_SOURCE_DIR}/mk/cmake/modules
	${CMAKE_SOURCE_DIR}/mk/cmake/modules/SDL2
)

include(CheckIncludeFiles)
include(GNUInstallDirs)
include(GetGitRevisionDescription)

set(CMAKE_CXX_STANDARD 11 CACHE STRING "C++ standard")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)

find_package(Threads)
find_package(ZLIB REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_gfx REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(LibXml2 REQUIRED)
# Prevent linking unneded libraries from LibXml2. see https://trac.macports.org/ticket/59436
set_target_properties(LibXml2::LibXml2 PROPERTIES INTERFACE_LINK_LIBRARIES "")
find_package(PhysFS MODULE REQUIRED) # config seems broken, so use module

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

git_describe_working_tree(FULL_PROJECT_VERSION --match lincity-ng-*)
if(FULL_PROJECT_VERSION)
	string(SUBSTRING ${FULL_PROJECT_VERSION} 11 -1 FULL_PROJECT_VERSION)
else()
	set(FULL_PROJECT_VERSION ${PROJECT_VERSION}-unknown)
endif()

check_include_files(dirent.h HAVE_DIRENT_H)

set(CMAKE_INSTALL_APPDATADIR ${CMAKE_INSTALL_DATADIR}/${CMAKE_PROJECT_NAME})
set(CMAKE_INSTALL_FULL_APPDATADIR ${CMAKE_INSTALL_FULL_DATADIR}/${CMAKE_PROJECT_NAME})
set(CMAKE_BINARY_BINDIR ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
set(CMAKE_BINARY_DATADIR ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_DATADIR})
set(CMAKE_BINARY_APPDATADIR ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_APPDATADIR})

add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)
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

if(NOT CMAKE_CROSSCOMPILING AND INCLUDE_WHAT_YOU_USE)
	set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE
		"${INCLUDE_WHAT_YOU_USE};-w;-Xiwyu;--mapping_file=${CMAKE_SOURCE_DIR}/iwyu-mapping")
	set(CMAKE_C_INCLUDE_WHAT_YOU_USE ${CMAKE_CXX_INCLUDE_WHAT_YOU_USE})
endif()

add_subdirectory(src)
add_subdirectory(data ${CMAKE_INSTALL_APPDATADIR})
add_subdirectory(doc)

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


set(CPACK_GENERATOR TGZ ZIP)
set(CPACK_PACKAGE_ICON data/lincity-ng.png)
set(CPACK_PACKAGE_CHECKSUM SHA256)
set(CPACK_RESOURCE_FILE_README README.md)
set(CPACK_SOURCE_GENERATOR TGZ)
set(CPACK_SOURCE_IGNORE_FILES ${CMAKE_BINARY_DIR}/ ${CMAKE_SOURCE_DIR}/\\\\.git/ ${CMAKE_SOURCE_DIR}/build.*/)
set(CPACK_PACKAGE_VERSION ${FULL_PROJECT_VERSION})
include(CPack)
