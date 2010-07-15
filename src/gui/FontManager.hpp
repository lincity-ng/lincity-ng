/*
Copyright (C) 2005 Matthias Braun <matze@braunis.de>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/**
 * @author Matthias Braun
 * @file FontManager.hpp
 */

#ifndef __FONTMANAGER_HPP__
#define __FONTMANAGER_HPP__

#include "Style.hpp"
#include <SDL_ttf.h>

/**
 * @class FontManager
 */
class FontManager
{
public:
    FontManager();
    ~FontManager();

    TTF_Font* getFont(Style style);

private:
    struct FontInfo {
        std::string name;
        int fontsize;
        int fontstyle;

        bool operator < (const FontInfo& other) const 
        {
            if(other.name < name)
                return true;
            if(other.name > name)
                return false;
            if(other.fontsize < fontsize)
                return true;
            if(other.fontsize > fontsize)
                return false;
            if(other.fontstyle < fontstyle)
                return true;
            return false;
        }

        bool operator ==(const FontInfo& other) const
        {
            return other.name == name && other.fontsize == fontsize
                && other.fontstyle == fontstyle;
        }
    };

    typedef std::map<FontInfo, TTF_Font*> Fonts;
    Fonts fonts;
};

extern FontManager* fontManager;

#endif


/** @file gui/FontManager.hpp */

