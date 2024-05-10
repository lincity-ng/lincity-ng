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
	PUBLIC
	$<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
	${PHYSFS_LIBRARY}
)

target_include_directories(physfsstream
	PUBLIC
	${PHYSFS_INCLUDE_DIR}
)

include_directories(.)
