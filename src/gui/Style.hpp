#ifndef __FONTSTYLE_HPP__
#define __FONTSTYLE_HPP__

#include <SDL.h>
#include <string>
#include <map>

#include "Color.hpp"

class XmlReader;

class Style
{
public:
    Style();
    ~Style();

    void parseAttributes(XmlReader& reader);
    bool parseAttribute(const char* name, const char* value);
    
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

private:
    Color parseColor(const char* value);
};

extern std::map<std::string, Style> styleRegistry;
void parseStyleDef(XmlReader& reader);

#endif

