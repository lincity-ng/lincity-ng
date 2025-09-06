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
 * @file Style.hpp
 */

#ifndef __FONTSTYLE_HPP__
#define __FONTSTYLE_HPP__

#include <map>        // for map
#include <string>     // for basic_string, string

#include "Color.hpp"  // for Color

namespace xmlpp {
class TextReader;
}  // namespace xmlpp

/**
 * @class Style
 */
//TODO make distinct styles for paragraphs and spans

class Style
{
public:
    Style();
    ~Style();

    bool parseAttribute(xmlpp::TextReader& reader);
    void parseAttributes(xmlpp::TextReader& reader);

    std::string href;

    std::string font_family;
    bool italic;
    bool bold;
    float font_size;
    Color text_color;
    Color background;

    // for boxes...
    enum Alignment {
        ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT
    };
    Alignment alignment;
    float margin_left, margin_right, margin_top, margin_bottom;
    float width, height, min_width, min_height;

    void toSpan(void); //restricts paragraph style to span style
private:
    Color parseColor(const char* value);
};

extern std::map<std::string, Style> styleRegistry;
void parseStyleDef(xmlpp::TextReader& reader);

#endif


/** @file gui/Style.hpp */
