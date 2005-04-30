#include <config.h>

#include <assert.h>
#include <iostream>

#include "FilledRectangle.hpp"
#include "XmlReader.hpp"
#include "Painter.hpp"
#include "ComponentFactory.hpp"

FilledRectangle::FilledRectangle()
{}

FilledRectangle::~FilledRectangle()
{}

void
FilledRectangle::parse(XmlReader& reader)
{
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(attribute, value)) {
            continue;
        } else if (strcmp(attribute, "color") == 0) {
            color.parse(value);
        } else {
            std::cerr << "Unknown attribute '" << attribute
                      << "' in FilledRectangle.\n";
        }
    }

    flags |= FLAG_RESIZABLE;
}

void
FilledRectangle::resize(float width, float height) 
{
    this->width = width;
    this->height = height;
}

void
FilledRectangle::draw(Painter& painter)
{
    painter.setFillColor(color);
    painter.fillRectangle(Rect2D(0, 0, width, height));
}

IMPLEMENT_COMPONENT_FACTORY(FilledRectangle)
