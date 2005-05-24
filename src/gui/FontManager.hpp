#ifndef __FONTMANAGER_HPP__
#define __FONTMANAGER_HPP__

#include "Style.hpp"
#include <SDL_ttf.h>

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
            if(other.fontsize < fontsize)
                return true;
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

