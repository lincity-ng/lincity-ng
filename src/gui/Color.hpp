#ifndef __COLOR_HPP__
#define __COLOR_HPP__

#include <stdint.h>
#include <SDL.h>

class Color
{
public:
    Color(uint8_t _r = 0, uint8_t _g = 0, uint8_t _b = 0, uint8_t _a = 255)
        : r(_r), g(_g), b(_b), a(_a)
    {
    }

    SDL_Color getSDLColor() const
    {
        SDL_Color result = { r, g, b, 0 };
        return result;
    }

    void parse(const char* value);
    
    uint8_t r, g, b, a;
};

#endif

