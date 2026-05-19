/* ---------------------------------------------------------------------- *
 * src/gui/FontManager.hpp
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

#ifndef __FONTMANAGER_HPP__
#define __FONTMANAGER_HPP__

#include <SDL3_ttf/SDL_ttf.h>  // for TTF_Font
#include <map>        // for map
#include <string>     // for basic_string, operator<, operator==, operator>

class Style;
class Vector2;

/**
 * @class FontManager
 */
class FontManager
{
public:
  FontManager();
  ~FontManager();

  TTF_Font* getFont(Style style, Vector2 scale);

private:
  struct FontInfo {
    std::string name;
    float fontsize;
    int hdpi, vdpi;
    int fontstyle;

    bool operator < (const FontInfo& other) const   {
      if(other.name < name)
        return true;
      if(other.name > name)
        return false;
      if(other.fontsize < fontsize)
        return true;
      if(other.fontsize > fontsize)
        return false;
      if(other.hdpi < hdpi)
        return true;
      if(other.hdpi > hdpi)
        return false;
      if(other.vdpi < vdpi)
        return true;
      if(other.vdpi > vdpi)
        return false;
      if(other.fontstyle < fontstyle)
        return true;
      return false;
    }

    bool operator ==(const FontInfo& other) const {
      return
        other.name == name &&
        other.fontsize == fontsize &&
        other.hdpi == hdpi &&
        other.vdpi == vdpi &&
        other.fontstyle == fontstyle;
    }
  };

  typedef std::map<FontInfo, TTF_Font*> Fonts;
  Fonts fonts;

  static constexpr int baseDpi = 72;
};

extern FontManager* fontManager;

#endif


/** @file gui/FontManager.hpp */
