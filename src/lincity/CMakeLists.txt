add_subdirectory(modules)

add_library(lincity_lib  STATIC
		all_buildings.hpp
		commodities.cpp
		commodities.hpp
		ConstructionRequest.cpp
		ConstructionRequest.hpp
		engine.cpp
		groups.hpp
		init_game.cpp
		init_game.hpp
		lc_locale.cpp
		lc_locale.hpp
		lin-city.hpp
		lintypes.cpp
		lintypes.hpp
		MapPoint.cpp
		MapPoint.hpp
		messages.cpp
		messages.hpp
		resources.cpp
		resources.hpp
		simulate.cpp
		stats.cpp
		stats.hpp
		sustainable.hpp
		transport.cpp
		transport.hpp
		Vehicles.cpp
		Vehicles.hpp
		world.cpp
		world.hpp
		util.cpp
		util.hpp
		xmlloadsave.cpp
		xmlloadsave.hpp
)

add_library(LinCity::lib ALIAS lincity_lib)

target_link_libraries(lincity_lib
		PUBLIC
		LinCity::lib_modules
		$<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
		ZLIB::ZLIB
		PkgConfig::libXMLPlusPlus
		PRIVATE
		tinygettext
		LinCity::gui
)

if (NOT WIN32)
	add_compile_definitions(lincity_lib PRIVATE DEFAULT_LIBDIR="lib")    # TODO
endif()

include_directories(.)
