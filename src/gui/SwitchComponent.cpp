#include <config.h>

#include "SwitchComponent.hpp"
#include "XmlReader.hpp"
#include "ComponentLoader.hpp"
#include "ComponentFactory.hpp"

SwitchComponent::SwitchComponent()
{
    setFlags(FLAG_RESIZABLE);
    activeComponentName = "";
}

SwitchComponent::~SwitchComponent()
{
}

void
SwitchComponent::parse(XmlReader& reader)
{
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(attribute, value)) {
            continue;
        } else {
            std::cerr << "Skipping unknown attribute '" << attribute << "'.\n";
        }
    }

    int depth = reader.getDepth();
    bool first = true;
    while(reader.read() && reader.getDepth() > depth) {
        if(reader.getNodeType() == XML_READER_TYPE_ELEMENT) {
            std::string element = (const char*) reader.getName();

            Component* component = createComponent(element, reader);
            Child& child = addChild(component);
            if(first) {
                child.enable(true);
                first = false;
                activeComponentName = child.getComponent()->getName(); 
            } else {
                child.enable(false);
            }
        }
    }
}

void
SwitchComponent::resize(float width, float height)
{
    for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
        Child& child = *i;
        if(child.getComponent() == 0) {
#ifdef DEBUG
            std::cerr << "Child in SwitchComponent==0 ?!?\n";
#endif
            continue;
        }
        if(! (child.getComponent()->getFlags() & FLAG_RESIZABLE))
            continue;
        child.getComponent()->resize(width, height);
    }
    this->width = width;
    this->height = height;
}

void
SwitchComponent::switchComponent(const std::string& name)
{
    bool found = false;
    for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
        Child& child = *i;
        if(child.getComponent()->getName() == name) {
            child.enable(true);
            found = true;
            activeComponentName = name;
        } else {
            child.enable(false);
        }
    }
    if(!found) {
#ifdef DEBUG
        std::cerr << "No component named '" << name << "' found "
            << "while switching components.\n";
#endif
        if(!childs.empty()) {
            childs[0].enable(true);
            activeComponentName = childs[0].getComponent()->getName(); 
        }
    }
    setDirty();
}

bool
SwitchComponent::opaque(const Vector2& pos) const
{
    for(Childs::const_iterator i = childs.begin(); i != childs.end(); ++i) {
        const Child& child = *i;
        if(child.getComponent() == 0 || !child.isEnabled())
            continue;

        if(child.getComponent()->opaque(pos - child.getPos()))
            return true;
    }
    
    return false;
}

IMPLEMENT_COMPONENT_FACTORY(SwitchComponent);
