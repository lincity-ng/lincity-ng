find_package(OpenGL REQUIRED)

add_library(lincity_gui OBJECT
		Button.cpp
		Child.cpp
		Color.cpp
		Component.cpp
		ComponentFactory.cpp
		ComponentLoader.cpp
		Desktop.cpp
		Document.cpp
		DocumentImage.cpp
		Event.cpp
		FilledRectangle.cpp
		Filter.cpp
		FontManager.cpp
		Gradient.cpp
		Image.cpp
		Panel.cpp
		Paragraph.cpp
		Rect2D.cpp
		ScrollBar.cpp
		ScrollView.cpp
		Style.cpp
		SwitchComponent.cpp
		TableLayout.cpp
		TextureManager.cpp
		TooltipManager.cpp
		Window.cpp
		XmlReader.cpp

		Button.hpp
		Child.hpp
		Color.hpp
		Component.hpp
		ComponentFactory.hpp
		ComponentLoader.hpp
		Desktop.hpp
		Document.hpp
		DocumentElement.hpp
		DocumentImage.hpp
		Event.hpp
		FilledRectangle.hpp
		Filter.hpp
		FontManager.hpp
		Gradient.hpp
		Image.hpp
		Painter.hpp
		Panel.hpp
		Paragraph.hpp
		Rect2D.hpp
		ScrollBar.hpp
		ScrollView.hpp
		Style.hpp
		SwitchComponent.hpp
		TableLayout.hpp
		Texture.hpp
		TextureManager.hpp
		TooltipManager.hpp
		Vector2.hpp
		Window.hpp
		XmlReader.hpp

		callback/Callback.hpp
		callback/CallbackBase.hpp
		callback/Signal.hpp

		PainterGL/PainterGL.cpp
		PainterGL/PainterGL.hpp
		PainterGL/TextureGL.cpp
		PainterGL/TextureGL.hpp
		PainterGL/TextureManagerGL.cpp
		PainterGL/TextureManagerGL.hpp

		PainterSDL/PainterSDL.cpp
		PainterSDL/PainterSDL.hpp
		PainterSDL/TextureSDL.cpp
		PainterSDL/TextureSDL.hpp
		PainterSDL/TextureManagerSDL.cpp
		PainterSDL/TextureManagerSDL.hpp
		)

add_library(LinCity::gui ALIAS lincity_gui)

target_link_libraries(lincity_gui
		PRIVATE
		tinygettext
		LinCity::physfsstream
		$<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
		SDL2::SDL2_gfx
		LibXml2::LibXml2
		OpenGL::GL
		)

target_include_directories(lincity_gui PRIVATE ${PHYSFS_INCLUDE_DIR})
target_link_libraries(lincity_gui PRIVATE ${PHYSFS_LIBRARY})
