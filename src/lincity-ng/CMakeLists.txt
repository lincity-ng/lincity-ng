add_executable(lincity-ng
		ButtonPanel.cpp
		ButtonPanel.hpp
		Config.cpp
		Config.hpp
		Dialog.cpp
		Dialog.hpp
		EconomyGraph.cpp
		EconomyGraph.hpp
		ErrorInterface.cpp
		ErrorInterface.hpp
		Game.cpp
		Game.hpp
		GameView.cpp
		GameView.hpp
		getBundleSharePath.cpp
		HelpWindow.cpp
		HelpWindow.hpp
		lc_error.c
		lc_error.h
		main.cpp
		main.hpp
		MainLincity.cpp
		MainLincity.hpp
		MainMenu.cpp
		MainMenu.hpp
		MapPoint.hpp
		MiniMap.cpp
		MiniMap.hpp
		Mps.cpp
		Mps.hpp
		PBar.cpp
		PBar.hpp
		readdir.cpp
		readdir.hpp
		ScreenInterface.cpp
		ScreenInterface.hpp
		Sound.cpp
		Sound.hpp
		TimerInterface.cpp
		TimerInterface.hpp
		UserOperation.cpp
		UserOperation.hpp
		Util.cpp
		Util.hpp
)

if (WIN32)
	set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_SOURCE_DIR}/contrib/win32/lincity-ng.rc")
	target_sources(lincity-ng PUBLIC ${APP_ICON_RESOURCE_WINDOWS})
endif()

target_link_libraries(lincity-ng
	PUBLIC
	LinCity::lib
	LinCity::gui
	$<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
	$<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
	PRIVATE
	tinygettext
	cfgpath
	$<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
	$<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
)

include_directories(.)
