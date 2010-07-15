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
 * @file Color.cpp
 */

#include <config.h>

#include "Color.hpp"

#include <sstream>
#include <stdexcept>
#include <stdio.h>

void
Color::parse(const char* value)
{
    if(value[0] == 0) {
        throw std::runtime_error("Can't parse empty string to color.");
    }

    if(value[0] == '#') {
        unsigned int red, green, blue, alpha;
        int res = sscanf(value, "#%2x%2x%2x%2x", &red, &green, &blue, &alpha);
        if(res < 3) {
            std::stringstream msg;
            msg << "Failed to parse color value '" << value << "'.";
            throw std::runtime_error(msg.str());
        } else {
            r = red;
            g = green;
            b = blue;
            if(res == 4)
                a = alpha;
            else
                a = 0xff;
        }
    } else if(strcmp(value, "black") == 0) {
        r = 0;
        g = 0;
        b = 0;
        a = 0xff;
    } else if(strcmp(value, "green") == 0) {
        r = 0;
        g = 0x80;
        b = 0;
        a = 0xff;
    } else if(strcmp(value, "white") == 0) {
        r = 0xff;
        g = 0xff;
        b = 0xff;
        a = 0xff;
    } else if(strcmp(value, "blue") == 0) {
        r = 0x00;
        g = 0x00;
        b = 0xff;
        a = 0xff;
    } else if(strcmp(value, "yellow") == 0) {
        r = 0xff;
        g = 0xff;
        b = 0;
        a = 0xff;
    } else if(strcmp(value, "red") == 0) {
        r = 0xff;
        g = 0;
        b = 0;
        a = 0xff;
    } else if(strcmp(value, "brown") == 0) {
        r = 165;
        g = 42;
        b = 42;
        a = 0xff;
    } else if(strcmp(value, "orange") == 0) {
        r = 255;
        g = 165;
        b = 0;
		} else if(strcmp(value, "gray") == 0) {
				r = 127;
				g = 127;
				b = 127;
    } else {
        std::stringstream msg;
        msg << "Unknown color name '" << value << "'.";
        throw std::runtime_error(msg.str());
    }
}

/** @file gui/Color.cpp */

