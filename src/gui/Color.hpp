#ifndef __COLOR_HPP__
#define __COLOR_HPP__

#include <stdint.h>
#include <SDL.h>

/**
 * This class defines a color in RGBA color space. You can specify a red, green,
 * blue and alpha value from 0 to 255. For red/green/blue a value of 0
 * means no saturation a value of 255 is full saturation.
 *
 * The alpha value does not change the color, but specifies the opacity of drawn
 * pixels. A value of 255 means full opacity a value of 0 means nothing is
 * drawn (= full transparent).
 */
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

    /** parse color values. Either specified in format #RRGGBBAA where
     * RR,GG,BB,AA are the red, green, blue, alpha values as hexadecimal
     * numbers.
     * Alternatively you can use predefined color names (like red,
     * black, yellow). Look in Color.cpp for a list of names.
     */
    void parse(const char* value);
    
    uint8_t r, g, b, a;
};

#endif

