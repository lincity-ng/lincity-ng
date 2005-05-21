#include <config.h>

#include "Child.hpp"
#include "Component.hpp"

Child::Child(Component* _component)
    : useClipRect(false), component(_component)
{ 
    enabled = component != 0;
}

Child::~Child()
{
}

bool
Child::inside(const Vector2& pos) const
{
    if(!enabled)
        return false;
    
    if(pos.x > position.x && pos.y > position.y
            && pos.x < position.x + component->getWidth()
            && pos.y < position.y + component->getHeight())
        return true;

    return false;
}

void
Child::setComponent(Component* component)
{
    delete this->component;
    this->component = component;
    enabled = component != 0;
}

//---------------------------------------------------------------------------

Childs::Childs()
{
}

Childs::~Childs()
{
    for(iterator i = begin(); i != end(); ++i)
        delete i->component;
}

Childs::iterator
Childs::erase(Childs::iterator i)
{
    delete i->component;
    return std::vector<Child>::erase(i);
}

void
Childs::clear()
{
    for(iterator i = begin(); i != end(); ++i)
        delete i->component;
    std::vector<Child>::clear();
}

