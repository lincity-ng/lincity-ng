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

    void resize(float width, float height);
    void draw(Painter& painter);
    
    std::string getFilename() const;
    void setFile(const std::string &filename);

private:
    std::auto_ptr<Texture> texture;
    bool tiling;
    std::string filename;
};

#endif

