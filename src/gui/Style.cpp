#include "Style.hpp"
#include "XmlReader.hpp"

#include <iostream>

Style::Style()
    : italic(false), bold(false), font_size(20),
    alignment(ALIGN_LEFT), margin_left(0), margin_right(0),
    margin_top(0), margin_bottom(0), width(-1), height(-1),
    min_width(-1), min_height(-1)
{
    font_family = "sans";
}

Style::~Style()
{
}

void
Style::parseStyleAttributes(XmlReader& reader)
{
    XmlReader::AttributeIterator iter(reader);

    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(!parseStyleAttribute(attribute, value)) {
            std::cerr << "Skipping unknown style attribute '"
                << attribute << "'.\n";
        }
    }
}

bool
Style::parseStyleAttribute(const char* attribute, const char* value)
{
    if(strcmp(attribute, "font-size") == 0) {
        if(sscanf(value, "%f", &font_size) != 1) {
            std::cerr << "Warning problem parsing size '" <<
                value << "'\n";
        }
    } else if(strcmp(attribute, "font-family") == 0) {
        font_family = value;
    } else if(strcmp(attribute, "font-style") == 0) {
        if(strcmp(value, "normal") == 0) {
            italic = false;
        } else if(strcmp(value, "italic") == 0) {
            italic = true;
        } else {
            std::cerr << "Invalid value for font-style "
                << "(only 'normal' and 'italic' allowed)\n";
        }
    } else if(strcmp(attribute, "font-weight") == 0) {
        if(strcmp(value, "normal") == 0) {
            bold = false;
        } else if(strcmp(value, "bold") == 0) {
            bold = true;
        } else {
            std::cerr << "Invalid value for font-weight "
                << "(only 'normal' and 'bold' allowed)\n";
        }
    } else if(strcmp(attribute, "text-align") == 0) {
        if(strcmp(value, "left") == 0) {
            alignment = Style::ALIGN_LEFT;
        } else if(strcmp(value, "center") == 0) {
            alignment = Style::ALIGN_CENTER;
        } else if(strcmp(value, "right") == 0) {
            alignment = Style::ALIGN_RIGHT;
        } else {
            std::cerr << "Invalid value for text-align attribute "
                << "(only 'left', 'center' and 'right' allowed)\n";
        }
    } else if(strcmp(attribute, "width") == 0) {
        if(sscanf(value, "%f", &width) != 1) {
            std::cerr << "Couldn't parse value for width: '"
                << value << "'\n";
        }
    } else if(strcmp(attribute, "height") == 0) {
        if(sscanf(value, "%f", &height) != 1) {
            std::cerr << "Couldn't parse value for height: '"
                << value << "'\n";
        }
    } else if(strcmp(attribute, "min-width") == 0) {
        if(sscanf(value, "%f", &min_width) != 1) {
            std::cerr << "Couldn't parse value for min-width: '"
                << value << "'\n";
        }                                                               
    } else if(strcmp(attribute, "min-height") == 0) {
        if(sscanf(value, "%f", &min_height) != 1) {
            std::cerr << "Couldn't parse value for min-height: '"
                << value << "'\n";
        }
    } else if(strcmp(attribute, "margin-left") == 0) {
        if(sscanf(value, "%f", &margin_left) != 1) {
            std::cerr << "Couldn't parse value for margin-left: '"
                << value << "'\n";
        }
    } else if(strcmp(attribute, "margin-right") == 0) {
        if(sscanf(value, "%f", &margin_right) != 1) {
            std::cerr << "Couldn't parse value for margin-right: '"
                << value << "'\n";
        }
    } else if(strcmp(attribute, "margin-top") == 0) {
        if(sscanf(value, "%f", &margin_top) != 1) {
            std::cerr << "Couldn't parse value for margin-top: '"
                << value << "'\n";
        }
    } else if(strcmp(attribute, "margin-bottom") == 0) {
        if(sscanf(value, "%f", &margin_bottom) != 1) {
            std::cerr << "Couldn't parse value for argin-bottom: '"
                << value << "'\n";
        }
    } else if(strcmp(attribute, "color") == 0) {
        text_color.parse(value);
    } else if(strcmp(attribute, "background") == 0) {
        background.parse(value);
    } else {
        return false;
    }

    return true;
}

