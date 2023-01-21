cmake_minimum_required(VERSION 3.21)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(X_VCPKG_APPLOCAL_DEPS_INSTALL ON)

project(lincity-ng
	VERSION 2.9.0.0
	LANGUAGES CXX
	DESCRIPTION "LinCity NG")

include(CheckIncludeFiles)
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

find_package(Threads)
find_package(ZLIB REQUIRED)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_gfx REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(PhysFS REQUIRED)

find_package(LibXslt REQUIRED)

check_include_files(dirent.h HAVE_DIRENT_H)

set(CMAKE_INSTALL_APPDATADIR ${CMAKE_INSTALL_DATADIR}/${CMAKE_PROJECT_NAME})

configure_file(gen/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/gen/config.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/gen)

add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)

add_subdirectory(src)
add_subdirectory(data)

install(TARGETS lincity-ng RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if (WIN32)
	install(FILES $<TARGET_PDB_FILE:lincity-ng> DESTINATION . OPTIONAL)
endif()

set(CPACK_GENERATOR "ZIP" CACHE STRING "Generators to support. semi-colon delimited list")
include(CPack)
