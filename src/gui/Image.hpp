#ifndef __IMAGE_HPP__
#define __IMAGE_HPP__

#include "Component.hpp"
#include <memory>

class XmlReader;
class Texture;

class Image : public Component
{
public:
    Image(Component* parent, XmlReader& reader);
    virtual ~Image();

    virtual void resize(float width, float height);
    virtual void draw(Painter& painter);
    virtual void event(Event& event);

private:
    std::auto_ptr<Texture> texture;
    bool tiling;
};

#endif

