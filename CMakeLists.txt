cmake_minimum_required(VERSION 3.21)
project(lincity-ng
	VERSION 2.9.0.0
	LANGUAGES CXX
	DESCRIPTION "LinCity NG")

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

configure_file(gen/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/gen/config.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/gen)

add_subdirectory(src)
