#include <config.h>

#include "DocumentImage.hpp"
#include "XmlReader.hpp"
#include "TextureManager.hpp"
#include "Painter.hpp"

DocumentImage::DocumentImage()
{}

DocumentImage::~DocumentImage()
{}

void
DocumentImage::parse(XmlReader& reader, const Style& parentstyle)
{
    style = parentstyle;
    
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(attribute, value)) {
            continue;
        } else if(style.parseAttribute(attribute, value)) {
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
            filename=value;
            texture.reset(texture_manager->load(value));
        } else {
            std::cerr << "Skipping unknown attribute '"
                << attribute << "'.\n";
        }
    }

    width = texture->getWidth();
    height = texture->getHeight();
}

void
DocumentImage::resize(float , float )
{}

void
DocumentImage::draw(Painter& painter)
{
    painter.drawTexture(texture.get(), Vector2(0, 0));
}
