#include <config.h>

#include "Image.hpp"
#include "XmlReader.hpp"
#include "TextureManager.hpp"
#include "ComponentFactory.hpp"
#include "Painter.hpp"

Image::Image()
{
}

Image::~Image()
{
}

void
Image::parse(XmlReader& reader)
{
    bool resizable = false;

    bool grey = false;

    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(attribute, value)) {
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
        } else if(strcmp(attribute, "filter") == 0) {
            if(strcmp(value, "grey") == 0) {
                grey = true;
            } else if(strcmp(value, "no") == 0) {
                grey = false;
            } else {
                std::cerr << "Unknown filter value '" << value << "'.\n";
                std::cerr << "Should be 'grey' or 'no'.\n";
            }
        } else if(strcmp(attribute, "resizable") == 0) {
            if(strcmp(value, "yes") == 0)
                resizable = true;
            else if(strcmp(value, "no") == 0)
                resizable = false;
            else
                std::cerr
                    << "You should specify 'yes' or 'no' for the resizable"
                    << "attribute\n";
        } else {
            std::cerr << "Skipping unknown attribute '"
                << attribute << "'.\n";
        }
    }

    if(filename == "")
        throw std::runtime_error("No filename specified for image");

    texture.reset(texture_manager->load(filename,
                                        grey ? TextureManager::FILTER_GREY
                                        : TextureManager::NO_FILTER));

    if(width <= 0 || height <= 0) {
        width = texture->getWidth();
        height = texture->getHeight();
    }

    if(resizable)
        flags |= FLAG_RESIZABLE;
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
    if(width != texture->getWidth() || height != texture->getHeight())
        painter.drawStretchTexture(texture.get(), Rect2D(0, 0, width, height));
    else
        painter.drawTexture(texture.get(), Vector2(0, 0));
}

std::string Image::getFilename() const
{
  return filename;
}
void Image::setFile(const std::string &pfilename)
{
  filename=pfilename;
  texture.reset(texture_manager->load(pfilename));
}

IMPLEMENT_COMPONENT_FACTORY(Image);

