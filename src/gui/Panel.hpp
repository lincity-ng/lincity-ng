#ifndef __PANEL_HPP__
#define __PANEL_HPP__

#include <memory>
#include "Component.hpp"

class XmlReader;
class Texture;

class Panel : public Component
{
public:
    Panel();
    virtual ~Panel();

    void parse(XmlReader& reader);

    void draw(Painter& painter);

private:
    std::auto_ptr<Texture> background;
};

#endif

