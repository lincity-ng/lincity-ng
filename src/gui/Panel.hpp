#ifndef __PANEL_HPP__
#define __PANEL_HPP__

#include <memory>
#include "Component.hpp"
#include "ComponentHolder.hpp"

class XmlReader;
class Texture;

class Panel : public Component
{
public:
    Panel(Component* parent, XmlReader& reader);
    virtual ~Panel();

    void draw(Painter& painter);

private:
    std::auto_ptr<Texture> background;
};

#endif

