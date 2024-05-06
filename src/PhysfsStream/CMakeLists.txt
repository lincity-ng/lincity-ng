add_library(physfsstream STATIC
		PhysfsSDL.cpp
		PhysfsSDL.hpp

		PhysfsStream.cpp
		PhysfsStream.hpp

		PhysfsError.hpp
		PhysfsError.cpp
		)
add_library(LinCity::physfsstream ALIAS physfsstream)

target_link_libraries(physfsstream
		PRIVATE
		$<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
		)

target_include_directories(physfsstream PRIVATE ${PHYSFS_INCLUDE_DIR})
target_link_libraries(physfsstream PRIVATE ${PHYSFS_LIBRARY})

include_directories(.)
