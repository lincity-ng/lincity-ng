#include <config.h>

#include "Component.hpp"

Component::Component(Component* _parent)
    : parent(_parent), flags(0), width(0), height(0)
{
}

Component::~Component()
{
}

bool
Component::parseAttributeValue(const char* attribute, const char* value)
{
    if(strcmp(attribute, "name") == 0) {
        name = value;
        return true;
    }

    return false;
}
