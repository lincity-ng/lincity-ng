#include "Image.hpp"
#include "XmlReader.hpp"
#include "TextureManager.hpp"
#include "ComponentFactory.hpp"
#include "Painter.hpp"

Image::Image(Component* parent, XmlReader& reader)
    : Component(parent)
{
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttributeValue(attribute, value)) {
            continue;
        } else if(strcmp(attribute, "width") == 0) {
            if(sscanf(value, "%f", &width) != 1) {
                std::stringstream msg;
                msg << "Couldn't parse width '" << value << "'.";
                throw std::runtime_error(msg.str());
            }
        } else if(strcmp(attribute, "height") == 0) {
            if(sscanf(value, "%f", &height) != 1) {
                std::stringstream msg;
                msg << "Couldn't parse height '" << value << "'.";
                throw std::runtime_error(msg.str());
            }
        } else if(strcmp(attribute, "src") == 0) {
            texture.reset(texture_manager->load(value));        
        } else {
            std::cerr << "Skipping unknown attribute '"
                << attribute << "'.\n";
        }
    }

    if(width <= 0 || height <= 0) {
        width = texture->getWidth();
        height = texture->getHeight();
    }
}

Image::~Image()
{
}

void
Image::resize(float width, float height)
{
    this->width = width;
    this->height = height;
}

void
Image::draw(Painter& painter)
{
    painter.drawTexture(texture.get(), Rect2D(0, 0, width, height));
}

IMPLEMENT_COMPONENT_FACTORY(Image)
