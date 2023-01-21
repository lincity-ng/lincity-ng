add_executable(lincity-ng
		ButtonInterface.cpp
		ButtonPanel.cpp
		ButtonPanel.hpp
		CheckButton.cpp
		CheckButton.hpp
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
		main.cpp
		main.hpp
		MainLincity.cpp
		MainLincity.hpp
		MainMenu.cpp
		MainMenu.hpp
		MapEdit.cpp
		MapEdit.hpp
		MapPoint.hpp
		MiniMap.cpp
		MiniMap.hpp
		Mps.cpp
		Mps.hpp
		MpsInterface.cpp
		PBar.cpp
		PBar.hpp
		PbarInterface.cpp
		Permutator.cpp
		Permutator.hpp
		readdir.cpp
		readdir.hpp
		ReadPngInterface.cpp
		ScreenInterface.cpp
		ScreenInterface.hpp
		Sound.cpp
		Sound.hpp
		TimerInterface.cpp
		TimerInterface.hpp
		Util.cpp
		Util.hpp
		)

set_target_properties(lincity-ng PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (WIN32)
	set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_SOURCE_DIR}/contrib/win32/lincity-ng.rc")
	target_sources(lincity-ng PUBLIC ${APP_ICON_RESOURCE_WINDOWS})
endif()

target_link_libraries(lincity-ng
		PRIVATE
		tinygettext
		LinCity::physfsstream
		LinCity::lib
		LinCity::gui
		PUBLIC
		$<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
		$<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
		)

target_include_directories(lincity-ng PRIVATE ${PHYSFS_INCLUDE_DIR})
target_link_libraries(lincity-ng PRIVATE ${PHYSFS_LIBRARY})

set(GENERATED_GUI_ROOT ${CMAKE_BINARY_DIR}/data/gui)
file(MAKE_DIRECTORY ${GENERATED_GUI_ROOT})

file(GLOB GuiSources ${CMAKE_SOURCE_DIR}/data/gui/dialogs/*.xml)

foreach(GuiSource ${GuiSources})
	get_filename_component(GuiSourceName ${GuiSource} NAME)
	set(GuiSourceOutput ${GENERATED_GUI_ROOT}/${GuiSourceName})
	add_custom_command(
			OUTPUT ${GuiSourceOutput}
			COMMAND LibXslt::xsltproc -o ${GuiSourceOutput} dialogs.xsl ${GuiSource}
			DEPENDS ${GuiSource}
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/data/gui/dialogs
			COMMENT "Convert XML GUI ${GuiSourceName}"
	)
	target_sources(lincity-ng PRIVATE ${GuiSourceOutput})
endforeach()