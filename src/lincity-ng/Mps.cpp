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
//#include "lincity/modules/all_modules.h"
#include "lincity/lctypes.h"

#include "Util.hpp"

#include "gui/XmlReader.hpp"
#include "gui/ComponentFactory.hpp"
#include "gui/ComponentLoader.hpp"
#include "gui/Paragraph.hpp"

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
    if( x < 0 || y < 0 || x >= WORLD_SIDE_LEN || y >= WORLD_SIDE_LEN )
        return;

    if (MP_TYPE(x,y) == CST_USED)
    {
        x = MP_INFO(x,y).int_1;
        y = MP_INFO(x,y).int_2;
    }

    // Used with MPS_GLOBAL, MPS_ENV and MPS_MAP
    clear();
    mps_update(x, y, style);
}

void
Mps::playBuildingSound(int mps_x, int mps_y)
{
    switch(MP_GROUP(mps_x, mps_y)) 
    {
        case GROUP_BLACKSMITH:
            getSound()->playSound( "Blacksmith" );
            break;
        case GROUP_COALMINE:
            getSound()->playSound( "CoalMine" );
            break;
        case GROUP_COAL_POWER:
            if( MP_TYPE( mps_x,mps_y) == CST_POWERS_COAL_FULL )
                getSound()->playSound( "PowerCoalFull" );
            else if( MP_TYPE( mps_x,mps_y) == CST_POWERS_COAL_MED )
                getSound()->playSound( "PowerCoalMed" );
            else if( MP_TYPE( mps_x,mps_y) == CST_POWERS_COAL_LOW )
                getSound()->playSound( "PowerCoalLow" );
            else //if( MP_TYPE( mps_x,mps_y) == CST_POWERS_COAL_EMPTY )
                getSound()->playSound( "PowerCoalEmpty" );
            break;
        case GROUP_COMMUNE:
            getSound()->playSound( "Commune" );
            break;
        case GROUP_CRICKET:
            getSound()->playSound( "SportsCroud" );
            break;
        case GROUP_FIRESTATION:
            getSound()->playSound( "FireStation" );
            break;
        case GROUP_HEALTH:
            getSound()->playSound( "Health" );
            break;
        case GROUP_INDUSTRY_H:
            getSound()->playSound( "IndustryHigh" );
            break;
        case GROUP_INDUSTRY_L:
            getSound()->playSound( "IndustryLight" );
            break;
        case GROUP_MILL:
            getSound()->playSound( "Mill" );
            break;
        case (GROUP_MONUMENT):
            if ((MP_INFO( mps_x,mps_y).int_1 * 100 / BUILD_MONUMENT_JOBS) >= 100) {
                getSound()->playSound( "Monument" );
            } else {
                getSound()->playSound( "MonumentConstruction" );
            }
            break;
        case (GROUP_OREMINE):
            getSound()->playSound( "OreMine" );
            break;
        case GROUP_ORGANIC_FARM: 
            getSound()->playSound( "OrganicFarm" );
            break;
        case GROUP_PORT:
            getSound()->playSound( "Harbor" );
            break;
        case GROUP_POTTERY:
            getSound()->playSound( "Pottery" );
            break;
        case GROUP_POWER_LINE:
            getSound()->playSound( "PowerLine" );
            break;
        case GROUP_RAIL:
        case GROUP_RAIL_BRIDGE:
            getSound()->playSound( "RailTrain" );
            break;
        case GROUP_RECYCLE:
            getSound()->playSound( "Recycle" );
            break;
        case GROUP_RESIDENCE_LL:
            getSound()->playSound( "ResidentialLowLow" );
            break;
        case GROUP_RESIDENCE_LH:
            getSound()->playSound( "ResidentialLow" );
            break;
        case GROUP_RESIDENCE_ML:
        case GROUP_RESIDENCE_MH:
            getSound()->playSound( "ResidentialMed" );
            break;
        case GROUP_RESIDENCE_HL:
        case GROUP_RESIDENCE_HH:
            getSound()->playSound( "ResidentialHigh" );
            break;
        case GROUP_ROAD:
        case GROUP_ROAD_BRIDGE:
            getSound()->playSound( "TraficLow" );
            //getSound()->playSound( "TraficHigh" );
            //TODO: find out when to use TraficHigh
            break;
        case GROUP_ROCKET:
            getSound()->playSound( "Rocket" );
            //TODO: what about RocketExplosion RocketTakeoff
            break;
        case GROUP_SCHOOL:
            getSound()->playSound( "School" );
            break;
        case GROUP_SOLAR_POWER:
            getSound()->playSound( "PowerSolar" );
            break;
        case GROUP_SUBSTATION:
            if( MP_TYPE( mps_x, mps_y )== CST_SUBSTATION_R ){
                getSound()->playSound( "SubstationOff" );
            } else if( MP_TYPE( mps_x, mps_y ) == CST_SUBSTATION_G ){
                getSound()->playSound( "SubstationOn" );
            } else{   //CST_SUBSTATION_RG
                getSound()->playSound( "Substation" );
            }
            break;
        case GROUP_TIP:
            getSound()->playSound( "Tip" );
            break;
        case GROUP_TRACK:
        case GROUP_TRACK_BRIDGE:
            getSound()->playSound( "DirtTrack" );
            break;
        case GROUP_MARKET:
            if( MP_TYPE( mps_x, mps_y ) == CST_MARKET_EMPTY )
                getSound()->playSound( "MarketEmpty" );
            else if( MP_TYPE( mps_x, mps_y ) == CST_MARKET_LOW )
                getSound()->playSound( "MarketLow" );
            else if( MP_TYPE( mps_x, mps_y ) == CST_MARKET_MED )
                getSound()->playSound( "MarketMed" );
            else //if( MP_TYPE( mps_x, mps_y ) == CST_MARKET_FULL )
                getSound()->playSound( "MarketFull" );

            break;
        case GROUP_UNIVERSITY:
            getSound()->playSound( "University" );
            break;
        case GROUP_WATER:
            getSound()->playSound( "Water" );
            break;
        case GROUP_WINDMILL:
            if( MP_TECH(mps_x, mps_y ) < MODERN_WINDMILL_TECH ){ 
                getSound()->playSound( "WindMill" );
            } else {
                getSound()->playSound( "WindMillHTech" );
            }
            break;
        case GROUP_FIRE:
            if( (MP_GROUP( mps_x, mps_y ) == GROUP_FIRE) &&
                   ( MP_INFO( mps_x, mps_y ).int_2 < FIRE_LENGTH ) ){
                getSound()->playSound( "Fire" );
            }
            break;
        case GROUP_SHANTY: 
            getSound()->playSound( "Shanty" );
            break;
        default: 
            if( GROUP_IS_BARE(MP_GROUP( mps_x, mps_y )) && (MP_GROUP( mps_x, mps_y) != GROUP_DESERT )){
                getSound()->playSound( "Green" );
            }  
            if( MP_TYPE( mps_x, mps_y ) == CST_PARKLAND_PLANE ){
                getSound()->playSound( "ParklandPlane" );
            }  
            if( MP_TYPE( mps_x, mps_y ) == CST_PARKLAND_LAKE ){
                getSound()->playSound( "ParklandLake" );
            }  
    }
}

int mps_set_silent(int x, int y, int style);

void mps_update(int mps_x, int mps_y, int mps_style)
{
    mps_set_silent(mps_x, mps_y, mps_style);
}

IMPLEMENT_COMPONENT_FACTORY(Mps);

