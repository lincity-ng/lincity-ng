/* ---------------------------------------------------------------------- *
 * src/gui/FontManager.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2005      Matthias Braun <matze@braunis.de>
 * Copyright (C) 2026      David Bears <dbear4q@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
** ---------------------------------------------------------------------- */

#include "FontManager.hpp"

#include <SDL3/SDL.h>             // for SDL_GetError
#include <SDL3_ttf/SDL_ttf.h>     // for TTF_CloseFont, TTF_Font, TTF_OpenFont
#include <filesystem>             // for path, operator/, operator<<
#include <sstream>                // for basic_stringstream, basic_ostream
#include <stdexcept>              // for runtime_error
#include <utility>                // for pair, make_pair
#include <fmt/format.h>

#include "Style.hpp"              // for Style
#include "lincity-ng/Config.hpp"  // for getConfig, Config
#include "Vector2.hpp"

FontManager* fontManager = 0;

FontManager::FontManager()
{
}

FontManager::~FontManager()
{
    for(Fonts::iterator i = fonts.begin(); i != fonts.end(); ++i)
        TTF_CloseFont(i->second);
}

TTF_Font*
FontManager::getFont(Style style, Vector2 scale) {
    FontInfo info;
    info.name = style.font_family;
    info.fontsize = style.font_size;
    info.hdpi = baseDpi * scale.x;
    info.vdpi = baseDpi * scale.y;
    info.fontstyle = 0;
    if(style.italic)
        info.fontstyle |= TTF_STYLE_ITALIC;
    if(style.bold)
        info.fontstyle |= TTF_STYLE_BOLD;

    if(info.hdpi < 1 || info.vdpi < 1)
      throw std::runtime_error(fmt::format(
        "FontManager::getFont: dpi (scale) is too small"));

    Fonts::iterator i = fonts.find(info);
    if(i != fonts.end())
        return i->second;

    std::filesystem::path fontsDir = getConfig()->appDataDir.get() / "fonts";
    std::filesystem::path fontfile = fontsDir / (info.name + ".ttf");
    TTF_Font* font = TTF_OpenFont(fontfile.string().c_str(), info.fontsize);
    if(!font) {
      std::stringstream msg;
      msg << "Error opening font " << fontfile
        << ": " << SDL_GetError();
      throw std::runtime_error(msg.str());
    }
    if(info.fontstyle != 0)
        TTF_SetFontStyle(font, info.fontstyle);
    if(!TTF_SetFontSizeDPI(font, info.fontsize, info.hdpi, info.vdpi))
      throw std::runtime_error(
        fmt::format("TTF_SetFontSizeDPI: {}", SDL_GetError()));

    fonts.insert(std::make_pair(info, font));
    return font;
}


/** @file gui/FontManager.cpp */
