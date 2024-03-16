cmake_minimum_required(VERSION 3.21)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(X_VCPKG_APPLOCAL_DEPS_INSTALL ON)

project(lincity-ng
	VERSION 2.10.1
	LANGUAGES CXX
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
find_package(PhysFS MODULE REQUIRED) # config seems broken, so use module
find_package(LibXslt REQUIRED)

git_describe_working_tree(GIT_TAG_VERSION --match lincity-ng-*)
if(GIT_TAG_VERSION)
	string(SUBSTRING ${GIT_TAG_VERSION} 11 -1 GIT_TAG_VERSION)
else()
	set(GIT_TAG_VERSION ${PROJECT_VERSION}-unknown)
endif()

check_include_files(dirent.h HAVE_DIRENT_H)

set(CMAKE_INSTALL_APPDATADIR ${CMAKE_INSTALL_DATADIR}/${CMAKE_PROJECT_NAME})
set(CMAKE_INSTALL_FULL_APPDATADIR ${CMAKE_INSTALL_FULL_DATADIR}/${CMAKE_PROJECT_NAME})
set(CMAKE_BINARY_BINDIR ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
set(CMAKE_BINARY_DATADIR ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_DATADIR})
set(CMAKE_BINARY_APPDATADIR ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_APPDATADIR})

add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)

add_subdirectory(src)
add_subdirectory(data ${CMAKE_INSTALL_APPDATADIR})
add_subdirectory(doc)

set_target_properties(lincity-ng PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_BINDIR})
install(TARGETS lincity-ng RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if (WIN32)
	install(FILES $<TARGET_PDB_FILE:lincity-ng> DESTINATION . OPTIONAL)
endif()


set(CPACK_GENERATOR TGZ ZIP)
set(CPACK_PACKAGE_ICON data/lincity-ng.png)
set(CPACK_PACKAGE_CHECKSUM SHA256)
set(CPACK_RESOURCE_FILE_README README.md)
set(CPACK_SOURCE_GENERATOR TGZ)
set(CPACK_SOURCE_IGNORE_FILES ${CMAKE_BINARY_DIR}/ ${CMAKE_SOURCE_DIR}/\\\\.git/)
include(CPack)
