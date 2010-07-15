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
 * @file TooltipManager.hpp
 */

#ifndef __TOOLTIPMANAGER_HPP__
#define __TOOLTIPMANAGER_HPP__

#include <map>
#include "Component.hpp"

class XmlReader;
class Paragraph;

static const Uint32 TOOLTIP_TIME = 500;

/**
 * @class TooltipManager
 */
class TooltipManager : public Component
{
public:
    TooltipManager();
    ~TooltipManager();

    void parse(XmlReader& reader);

    void resize(float width, float height);
    void event(const Event& event);
    bool opaque(const Vector2& pos) const;

    void showTooltip(const std::string& text, const Vector2& pos);

private:
    Child& comp_tooltip()
    {
        return childs[0];
    }
};

/// global TooltipManager instance
extern TooltipManager* tooltipManager;

#endif


/** @file gui/TooltipManager.hpp */

