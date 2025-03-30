add_subdirectory(modules)

add_library(lincity_lib  STATIC
		all_buildings.cpp
		all_buildings.h
		commodities.cpp
		commodities.hpp
		ConstructionRequest.cpp
		ConstructionRequest.h
		engglobs.cpp
		engglobs.h
		engine.cpp
		groups.h
		init_game.cpp
		init_game.h
		lc_locale.cpp
		lc_locale.h
		lc_random.cpp
		lc_random.hpp
		lclib.cpp
		lclib.h
		lctypes.h
		lin-city.h
		lintypes.cpp
		lintypes.h
		MapPoint.cpp
		MapPoint.hpp
		messages.cpp
		messages.hpp
		resources.cpp
		resources.hpp
		simulate.cpp
		stats.cpp
		stats.h
		sustainable.h
		transport.cpp
		transport.h
		Vehicles.cpp
		Vehicles.h
		world.cpp
		world.h
		xmlloadsave.cpp
		xmlloadsave.h
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
