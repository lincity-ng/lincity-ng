cmake_minimum_required(VERSION 3.21)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(X_VCPKG_APPLOCAL_DEPS_INSTALL ON)

project(lincity-ng
	VERSION 2.9.0.0
	LANGUAGES CXX
	DESCRIPTION "LinCity NG")

include(CheckIncludeFiles)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads)
find_package(ZLIB REQUIRED)

find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-gfx CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(PhysFS REQUIRED)

check_include_files(dirent.h HAVE_DIRENT_H)

configure_file(gen/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/gen/config.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/gen)

add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)

add_subdirectory(src)

install(TARGETS lincity-ng RUNTIME DESTINATION .)

if (WIN32)
	install(FILES $<TARGET_PDB_FILE:lincity-ng> DESTINATION . OPTIONAL)
endif()

install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/fonts DESTINATION data)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/fonts DESTINATION data)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/gui DESTINATION data)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/help DESTINATION data)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/images DESTINATION data)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/locale DESTINATION data)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/music DESTINATION data)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/opening DESTINATION data FILES_MATCHING PATTERN "*.scn")
install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/sounds DESTINATION data)

install(FILES ${CMAKE_SOURCE_DIR}/data/colour.pal DESTINATION data)
install(FILES ${CMAKE_SOURCE_DIR}/data/lincityconfig.xml DESTINATION data)

set(CPACK_GENERATOR "ZIP" CACHE STRING "Generators to support. semi-colon delimited list")
include(CPack)