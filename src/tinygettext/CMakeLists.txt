add_library(tinygettext
		findlocale.cpp
		findlocale.hpp
		gettext.hpp
		tinygettext.cpp
		tinygettext.hpp
		)

target_link_libraries(tinygettext
		PRIVATE
		$<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
		)