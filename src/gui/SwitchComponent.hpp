#ifndef __SWITCH_COMPONENT_HPP__
#define __SWITCH_COMPONENT_HPP__

#include <string>
#include "Component.hpp"

class XmlReader;

/** This component keeps a list of several components and can dynamically switch
 * between them. This is usefull to implement tabbed dialogs.
 */
class SwitchComponent : public Component
{
public:
    SwitchComponent();
    virtual ~SwitchComponent();

    void parse(XmlReader& reader);
    void resize(float width, float height);
    bool opaque(const Vector2& pos) const;

    void switchComponent(const std::string& name);
    Component* getActiveComponent();
};

#endif

