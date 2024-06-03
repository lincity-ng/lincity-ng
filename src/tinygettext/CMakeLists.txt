add_library(tinygettext STATIC
		findlocale.cpp
		findlocale.hpp
		gettext.hpp
		tinygettext.cpp
		tinygettext.hpp
)

target_link_libraries(tinygettext
		PRIVATE
		LinCity::physfsstream
		$<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

include_directories(.)
