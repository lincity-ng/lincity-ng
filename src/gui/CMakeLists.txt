set(LINCITYNG_DISABLE_GL_MODE ON CACHE BOOL "Disable direct OpenGL renderer.")

if(NOT LINCITYNG_DISABLE_GL_MODE)
  find_package(OpenGL REQUIRED)
endif()

add_library(lincity_gui  STATIC
  Button.cpp
  CheckButton.cpp
  Child.cpp
  Color.cpp
  Component.cpp
  ComponentFactory.cpp
  ComponentLoader.cpp
  Desktop.cpp
  DialogBuilder.cpp
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
  RadioButtonGroup.cpp
  Rect2D.cpp
  ScrollBar.cpp
  ScrollView.cpp
  Style.cpp
  SwitchComponent.cpp
  TableLayout.cpp
  TextureManager.cpp
  TooltipManager.cpp
  Vector2.cpp
  Window.cpp
  WindowManager.cpp

  Button.hpp
  CheckButton.hpp
  Child.hpp
  Color.hpp
  Component.hpp
  ComponentFactory.hpp
  ComponentLoader.hpp
  Desktop.hpp
  DialogBuilder.hpp
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
  RadioButtonGroup.hpp
  Rect2D.hpp
  ScrollBar.hpp
  ScrollView.hpp
  Signal.hpp
  Style.hpp
  SwitchComponent.hpp
  TableLayout.hpp
  Texture.hpp
  TextureManager.hpp
  TooltipManager.hpp
  Vector2.hpp
  Window.hpp
  WindowManager.hpp

  # PainterGL/PainterGL.cpp
  # PainterGL/PainterGL.hpp
  # PainterGL/TextureGL.cpp
  # PainterGL/TextureGL.hpp
  # PainterGL/TextureManagerGL.cpp
  # PainterGL/TextureManagerGL.hpp

  PainterSDL/PainterSDL.cpp
  PainterSDL/PainterSDL.hpp
  PainterSDL/TextureSDL.cpp
  PainterSDL/TextureSDL.hpp
  PainterSDL/TextureManagerSDL.cpp
  PainterSDL/TextureManagerSDL.hpp
)

if(NOT LINCITYNG_DISABLE_GL_MODE)
  target_sources(lincity_gui PRIVATE
    PainterGL/PainterGL.cpp
    PainterGL/PainterGL.hpp
    PainterGL/TextureGL.cpp
    PainterGL/TextureGL.hpp
    PainterGL/TextureManagerGL.cpp
    PainterGL/TextureManagerGL.hpp
  )
endif()

if(LINCITYNG_DISABLE_GL_MODE)
	target_compile_definitions(lincity_gui PUBLIC DISABLE_GL_MODE)
endif()

add_library(LinCity::gui ALIAS lincity_gui)

target_link_libraries(lincity_gui
  PUBLIC
  $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
  $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
  xmlutil
  LibXml++-5.0::LibXml++-5.0
  PRIVATE
  fmt::fmt
  gettext
  ptrutil
)
if(NOT LINCITYNG_DISABLE_GL_MODE)
  target_link_libraries(lincity_gui PUBLIC OpenGL::GL)
endif()

target_include_directories(lincity_gui
  PUBLIC
  ${LIBXML2_INCLUDE_DIR}
)

include_directories(.)
