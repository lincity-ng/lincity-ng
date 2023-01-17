add_subdirectory(modules)

add_library(lincity_lib
		OBJECT

		all_buildings.cpp
		all_buildings.h
		ConstructionCount.cpp
		ConstructionCount.h
		ConstructionManager.cpp
		ConstructionManager.h
		ConstructionRequest.cpp
		ConstructionRequest.h
		engglobs.cpp
		engglobs.h
		engine.cpp
		engine.h
		fileutil.cpp
		fileutil.h
		groups.h
		init_game.cpp
		init_game.h
		lc_locale.cpp
		lc_locale.h
		lclib.cpp
		lclib.h
		lctypes.h
		lin-city.h
		lintypes.cpp
		lintypes.h
		loadsave.cpp
		loadsave.h
		old_ldsvguts.cpp
		old_ldsvguts.h
		simulate.cpp
		simulate.h
		stats.cpp
		stats.h
		sustainable.h
		transport.cpp
		transport.h
		UserOperation.cpp
		UserOperation.h
		Vehicles.cpp
		Vehicles.h
		world.cpp
		world.h
		xmlloadsave.cpp
		xmlloadsave.h
		)

add_library(LinCity::lib ALIAS lincity_lib)

target_link_libraries(lincity_lib
		PRIVATE
		tinygettext
		LinCity::physfsstream
		LinCity::lib_modules
		$<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
		)

target_include_directories(lincity_lib PRIVATE ${PHYSFS_INCLUDE_DIR})
target_link_libraries(lincity_lib PRIVATE ${PHYSFS_LIBRARY})

if (NOT WIN32)
	add_compile_definitions(lincity_lib PRIVATE DEFAULT_LIBDIR="lib")    # TODO
endif()