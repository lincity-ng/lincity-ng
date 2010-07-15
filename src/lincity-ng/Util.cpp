/*
Copyright (C) 2005 Matthias Braun <matze@braunis.de>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include <config.h>

#include "Util.hpp"

#include <stdexcept>
#include <sstream>
#include <typeinfo>

#include "gui/Button.hpp"
#include "gui/Component.hpp"
#include "gui/Paragraph.hpp"
#include "gui/SwitchComponent.hpp"

#include "CheckButton.hpp"

template<typename T>
void findComponent(T** result, Component& tree, const std::string& name)
{
    Component* component = tree.findComponent(name);
#ifdef DEBUG
    if(!component) {
        std::stringstream msg;
        msg << "GUI file didn't define '" << name << "' (type "
            << typeid(T).name() << ")";
        throw std::runtime_error(msg.str());
    }
#endif
    T* casted_component = dynamic_cast<T*> (component);
#ifdef DEBUG
    if(!casted_component) {
        std::stringstream msg;
        msg << "Component '" << name << "' is of type "
            << typeid(*component).name() << " but "
            << typeid(T).name() << " is expected.";
        throw std::runtime_error(msg.str());
    }
#endif
    *result = casted_component;
}

Button* getButton(Component& tree, const std::string& name)
{
    Button* result;
    findComponent(&result, tree, name);
    return result;
}
CheckButton* getCheckButton(Component& tree, const std::string& name)
{
    CheckButton* result;
    findComponent(&result, tree, name);
    return result;
}

Paragraph* getParagraph(Component& tree, const std::string& name)
{
    Paragraph* result;
    findComponent(&result, tree, name);
    return result;
}

SwitchComponent* getSwitchComponent(Component& tree, const std::string& name)
{
    SwitchComponent* result;
    findComponent(&result, tree, name);
    return result;
}

/** @file lincity-ng/Util.cpp */

