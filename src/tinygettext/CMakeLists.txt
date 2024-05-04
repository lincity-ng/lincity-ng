add_library(tinygettext OBJECT
		findlocale.cpp
		findlocale.hpp
		gettext.hpp
		tinygettext.cpp
		tinygettext.hpp
)

target_link_libraries(tinygettext
		PRIVATE
		physfsstream
		$<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

include_directories(.)
