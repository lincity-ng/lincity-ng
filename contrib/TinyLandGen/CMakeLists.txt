add_executable(TinyLandGen
		ecology.cpp
		ground_sea.cpp
		lakes_rivers.cpp
		lctypes.h
		main.cpp
		save.cpp
		)

target_link_libraries(TinyLandGen
		PRIVATE
		ZLIB::ZLIB
		$<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
		$<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

include_directories(.)
