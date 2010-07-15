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

/**
 * @author Matthias Braun
 * @file SwitchComponent.hpp
 */

#ifndef __SWITCH_COMPONENT_HPP__
#define __SWITCH_COMPONENT_HPP__

#include <string>
#include "Component.hpp"

class XmlReader;

/**
 * @class SwitchComponent
 * @author Matthias Braun
 * @brief Component for component switch (used to implement tabbed dialogs).
 *
 * This component keeps a list of several components and
 * can dynamically switch between them. This is usefull to implement tabbed
 * dialogs.
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


/** @file gui/SwitchComponent.hpp */

