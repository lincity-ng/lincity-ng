/*
Copyright (C) 2005 David Kamphausen <david.kamphausen@web.de>

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

#include "Mps.hpp"
#include "gui_interface/mps.h"
#include "lincity/engglobs.h"
#include "lincity/lctypes.h"
#include "Util.hpp"

#include "gui/XmlReader.hpp"
#include "gui/ComponentFactory.hpp"
#include "gui/ComponentLoader.hpp"
#include "gui/Paragraph.hpp"
#include "lincity/modules/all_modules.h"

#include "Sound.hpp"

Mps* globalMPS = 0;
Mps* mapMPS = 0;
Mps* envMPS = 0;

Mps::Mps()
{
}

Mps::~Mps()
{
}

void
Mps::parse(XmlReader& reader)
{
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* name = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(name, value)) {
            continue;
        } else {
            std::cerr << "Unknown attribute '" << name
                      << "' skipped in Mps.\n";
        }
    }

    if(getName() == "GlobalMPS") {
        globalMPS = this;
    } else if(getName() == "MapMPS") {
        mapMPS = this;
    } else if(getName() == "EnvMPS") {
        envMPS = this;
    } else {
        std::cerr << "Unknown MPS component '" << getName() << "' found.\n";
    }

    Component* component = parseEmbeddedComponent(reader);
    addChild(component);
    width = component->getWidth();
    height = component->getHeight();

    for(int i = 0; i < MPS_PARAGRAPH_COUNT; ++i) {
        std::ostringstream compname;
        compname << "mps_text" << i;
        Paragraph* p = getParagraph(*this, compname.str());
        paragraphs.push_back(p);
    }

    setView(MapPoint(10,10));
}

void
Mps::setText(int i, const std::string &s)
{
    assert(i >= 0 && i < MPS_PARAGRAPH_COUNT);
    paragraphs[i]->setText(s);
}

void
Mps::clear()
{
    for(Paragraphs::iterator i = paragraphs.begin(); i != paragraphs.end(); ++i)
        (*i)->setText("");
}

void
Mps::setView(MapPoint point, int style /* = MPS_MAP */ )
{
    int x = point.x;
    int y = point.y;
    if( !world.is_inside(x, y)) {return;}
    Construction *reportingConstruction = world(x,y)->reportingConstruction;
    if (reportingConstruction) {
        x = reportingConstruction->x;
        y = reportingConstruction->y;
    }

    // Used with MPS_GLOBAL, MPS_ENV and MPS_MAP
    clear();
    mps_update(x, y, style);
}

void
Mps::playBuildingSound(int mps_x, int mps_y)
{
    if(world(mps_x, mps_y)->reportingConstruction)
    {
        unsigned short group = world(mps_x, mps_y)->reportingConstruction->constructionGroup->group;
        if( group == GROUP_ROAD || group == GROUP_ROAD_BRIDGE)
        {   dynamic_cast<Transport *>(world(mps_x, mps_y)->reportingConstruction)->playSound();}
        else
        {   world(mps_x, mps_y)->reportingConstruction->playSound();}
    }
    else
    {
        ConstructionGroup* constructionGroup = world(mps_x, mps_y)->getTileConstructionGroup();
        int s = constructionGroup->chunks.size();
        if(s)
        {   getSound()->playASound( constructionGroup->chunks[ rand()%s ] );}
    }
    std::cout.flush();
}

int mps_set_silent(int x, int y, int style);

void mps_update(int mps_x, int mps_y, int mps_style)
{
    mps_set_silent(mps_x, mps_y, mps_style);
}

IMPLEMENT_COMPONENT_FACTORY(Mps)


/** @file lincity-ng/Mps.cpp */

