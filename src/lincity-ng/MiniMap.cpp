/* ---------------------------------------------------------------------- *
 * src/lincity-ng/MiniMap.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2005      David Kamphausen <david.kamphausen@web.de>
 * Copyright (C) 2024-2025 David Bears <dbear4q@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
** ---------------------------------------------------------------------- */

#include "MiniMap.hpp"

#include <SDL.h>                          // for Uint16, Uint8, SDL_BUTTON_R...
#include <assert.h>                       // for assert
#include <stdio.h>                        // for sscanf, size_t
#include <string.h>                       // for strlen, strcmp
#include <array>                          // for array
#include <functional>                     // for bind, _1, function, _2
#include <iostream>                       // for basic_ostream, operator<<
#include <sstream>                        // for basic_stringstream
#include <stdexcept>                      // for runtime_error

#include "Dialog.hpp"                     // for Dialog, ASK_COAL_SURVEY
#include "Game.hpp"                       // for Game
#include "GameView.hpp"                   // for GameView
#include "MainLincity.hpp"                // for setSimulationDelay
#include "Mps.hpp"                        // for MpsFinance, MpsMap
#include "Util.hpp"                       // for getCheckButton, getButton
#include "gui/Button.hpp"                 // for Button
#include "gui/CheckButton.hpp"            // for CheckButton
#include "gui/ComponentFactory.hpp"       // for IMPLEMENT_COMPONENT_FACTORY
#include "gui/Event.hpp"                  // for Event
#include "gui/Painter.hpp"                // for Painter
#include "gui/Rect2D.hpp"                 // for Rect2D
#include "gui/Signal.hpp"                 // for Signal
#include "gui/SwitchComponent.hpp"        // for SwitchComponent
#include "gui/Texture.hpp"                // for Texture
#include "gui/TextureManager.hpp"         // for TextureManager, texture_man...
#include "gui/XmlReader.hpp"              // for XmlReader
#include "lincity/all_buildings.h"        // for COAL_RESERVE_SIZE
#include "lincity/commodities.hpp"        // for Commodity, CommodityRule
#include "lincity/groups.h"               // for GROUP_POWER_LINE, GROUP_FIRE
#include "lincity/lin-city.h"             // for FLAG_IS_TRANSPORT, FLAG_CRI...
#include "lincity/lintypes.h"             // for Construction, ConstructionG...
#include "lincity/modules/all_modules.h"  // for Powerline, Transport, Fire
#include "lincity/transport.h"            // for TRANSPORT_QUANTA, TRANSPORT...
#include "lincity/world.h"                // for Map, MapTile, World

using namespace std::placeholders;


/** List of mapview buttons. The "" entries separate mapview buttons that are
 * switched
 */
const char* mapViewButtons[] = {
    "MapViewNormal", "", "MapViewFood", "", "MapViewUB40", "", "MapViewPower", "",
    "MapViewFire", "", "MapViewSport", "", "MapViewHealth", "", "MapViewTraffic", "",
    "MapViewPollution", "", "MapViewCoal", "", 0};

const char* speedButtons[] = {
    "SpeedPauseButton", "SpeedNormalButton", "SpeedFastButton",
    "SpeedFastestButton", 0 };

static inline Uint8 brightness(const Color &c)
{
    Uint16 x=c.r;
    x+=c.g;
    x+=c.b;
    return x/3;
}

static inline Color makeGrey(const Color &c)
{
    Uint8 b=brightness(c);
    return Color(b,b,b);
}

static inline Color makeBlue(const Color &c)
{
    Uint8 b=brightness(c);
    return Color(0,0,b);
}

static inline Color light(const Color &c,Uint8 b)
{
    return Color(
            ((Uint16(c.r)*b)>>8),
            ((Uint16(c.g)*b)>>8),
            ((Uint16(c.b)*b)>>8));
}

MiniMap* miniMapPtr = 0;

MiniMap* getMiniMap()
{
    return miniMapPtr;
}

MiniMap::MiniMap()
    : mMode(NORMAL), tilesize(2), border(0), mTexture(nullptr)
{
    assert( miniMapPtr == 0 );
    miniMapPtr = this;
}

MiniMap::~MiniMap()
{
    if(miniMapPtr == this)
        miniMapPtr = 0;
}

void
MiniMap::setGame(Game *game) {
  this->game = game;
}

Commodity
MiniMap::getStuffID()
{
    return stuff_ID;
}

void
MiniMap::toggleStuffID(int step)
{
    static const Commodity commodities[]  =
    {STUFF_FOOD,STUFF_LABOR,
    STUFF_COAL,STUFF_ORE,
    STUFF_GOODS,STUFF_STEEL,
    STUFF_WASTE,STUFF_LOVOLT,
    STUFF_HIVOLT,STUFF_WATER};

    //check if we are at the beginning or the end of commodities
    if (step == 1 && stuff_ID == STUFF_WATER)
    {
        stuff_ID = STUFF_FOOD;
    }
    else if (step != 1 && stuff_ID == STUFF_FOOD)
    {
        stuff_ID = STUFF_WATER;
    }
    else //go a step forth or back
    {
        size_t pos = 0;
        while (commodities[pos] != stuff_ID && pos < sizeof(commodities)/sizeof(stuff_ID))
        {   ++pos;}
        stuff_ID = commodities[pos+step];
    }
    game->getGameView().setMapMode( mMode );
}

World&
MiniMap::getWorld() const {
  return game->getWorld();
}




void
MiniMap::parse(XmlReader& reader)
{
    // parse attributes...
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* name = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(name, value)) {
            continue;
        } else if(strcmp(name, "width") == 0) {
            if(sscanf(value, "%f", &width) != 1) {
                std::stringstream msg;
                msg << "Couldn't parse width attribute (" << value << ").";
                throw std::runtime_error(msg.str());
            }
        } else if(strcmp(name, "height") == 0) {
            if(sscanf(value, "%f", &height) != 1) {
                std::stringstream msg;
                msg << "Couldn't parse height attribute (" << value << ").";
                throw std::runtime_error(msg.str());
            }
        } else if(strcmp(name, "border") == 0) {
            if(sscanf(value, "%d", &border) != 1) {
                std::stringstream msg;
                msg << "Couldn't parse border attribute (" << value << ").";
                throw std::runtime_error(msg.str());
            }
        } else if(strcmp(name, "tilesize") == 0) {
            if(sscanf(value, "%d", &tilesize) != 1) {
                std::stringstream msg;
                msg << "Couldn't parse tilesize attribute (" << value << ").";
                throw std::runtime_error(msg.str());
            }
        } else {
            std::cerr << "Unknown attribute '" << name << "' skipped.\n";
        }
    }
    if(width <= 0 || height <= 0)
      throw std::runtime_error("Width or Height invalid");

    // create alpha-surface
    SDL_Surface* image = SDL_CreateRGBSurface(0, (int) width, (int) height, 32,
                          0x000000ff, 0x0000ff00,
                                              0x00ff0000, 0xff000000);
    mTexture.reset(texture_manager->create(image));

    mFullRefresh=true;
    alreadyAttached=false;
    inside = false;

    this->stuff_ID = STUFF_FOOD;
}

Component *MiniMap::findRoot(Component *c)
{
    while(c->getParent())
    {   return findRoot(c->getParent());}
    return c;
}

void MiniMap::attachButtons()
{
    if(alreadyAttached)
    {   return;}
    alreadyAttached=true;

    Component *root=findRoot(this);

    for(int i = 0; mapViewButtons[i] != 0; ++i)
    {
        if(strlen(mapViewButtons[i]))
        {
            CheckButton* b = getCheckButton(*root, mapViewButtons[i]);
            if(i == 0)
            {   b->check();}
            b->clicked.connect(std::bind(&MiniMap::mapViewButtonClicked, this, _1, _2));
        }
    }

    // TODO: move these non-minimap components out of here
    for(int i = 0; speedButtons[i] != 0; ++i) {
        CheckButton* b = getCheckButton(*root, speedButtons[i]);
        if(i == 1)
        {   b->check();}
        b->clicked.connect(std::bind(&MiniMap::speedButtonClicked, this, _1, _2));
    }

    Button* zoomInButton = getButton(*root, "ZoomInButton");
    zoomInButton->clicked.connect(std::bind(&MiniMap::zoomInButtonClicked, this, _1));
    Button* zoomOutButton = getButton(*root, "ZoomOutButton");
    zoomOutButton->clicked.connect(std::bind(&MiniMap::zoomOutButtonClicked, this, _1));

    CheckButton* switchMinimapButton = getCheckButton(*root, "SwitchMiniMap");
    switchMinimapButton->clicked.connect(std::bind(&MiniMap::switchButton, this, _1, _2));
    switchButtons.push_back(switchMinimapButton);

    CheckButton* switchPBarButton = getCheckButton(*root, "SwitchPBar");
    switchPBarButton->clicked.connect(std::bind(&MiniMap::switchButton, this, _1, _2));
    switchButtons.push_back(switchPBarButton);

    CheckButton* switchButton = getCheckButton(*root, "SwitchGlobalMPS");
    switchButton->clicked.connect(std::bind(&MiniMap::switchButton, this, _1, _2));
    switchButtons.push_back(switchButton);

    switchButton = getCheckButton(*root, "SwitchEconomyGraph");
    switchButton->clicked.connect(std::bind(&MiniMap::switchButton, this, _1, _2));
    switchButtons.push_back(switchButton);

    Button* scrollPageDown = getButton(*root, "ScrollPageDown");
    scrollPageDown->clicked.connect(std::bind(&MiniMap::scrollPageDownButtonClicked, this, _1));
    Button* scrollPageUp = getButton(*root, "ScrollPageUp");
    scrollPageUp->clicked.connect(std::bind(&MiniMap::scrollPageUpButtonClicked, this, _1));

}

void
MiniMap::switchButton(CheckButton* button, int mousebutton)
{
    if( !alreadyAttached )
    {   return;}
    if(mousebutton == SDL_BUTTON_RIGHT )
    {   game->showHelpWindow( "dialogs" );}
    std::string active_button_name = "Switch" + getSwitchComponent(*(findRoot(this)), "MiniMapSwitch")->getActiveComponent()->getName();
    std::string buttonName = button->getName();
    // remove "Switch" prefix
    std::string switchname = std::string(buttonName, 6, buttonName.size()-1);
    switchView(switchname);
    if(buttonName == active_button_name)
      scrollPageDown(true);
}

void
MiniMap::switchView(const std::string& viewname)
{
    SwitchComponent* switchComponent
        = getSwitchComponent(*(findRoot(this)), "MiniMapSwitch");

    switchComponent->switchComponent(viewname);
/*
    if (viewname != "EconomyGraph")
    {   getGameView()->economyGraph_open = false;}
*/
    //TODO once this gets more complex PBar pages could become
    //a nested switchComponent. For now only one case ;-)
    std::string viewGroupName = viewname;
    if (viewGroupName == "PBar2nd")
    {   viewGroupName = "PBar";}

    std::string buttonname = "Switch";
    buttonname += viewGroupName;

    for(std::vector<CheckButton*>::iterator i = switchButtons.begin();
            i != switchButtons.end(); ++i)
    {
        CheckButton* cbutton = *i;
        if(cbutton->getName() == buttonname)
        {   cbutton->check();}
        else
        {   cbutton->uncheck();}
    }
}

void
MiniMap::switchMapViewButton(const std::string &buttonName)
{
    return; //no switch atm
    std::string switchName;
    if(buttonName=="MapViewFood" || buttonName=="MapViewUB40") {
        switchName = "FoodSwitch";
    } else if(buttonName=="MapViewTraffic" || buttonName=="MapViewPollution") {
        switchName = "TrafficSwitch";
    } else if(buttonName=="MapViewPower" || buttonName=="MapViewHealth"
            || buttonName=="MapViewFire" || buttonName=="MapViewSport") {
        switchName = "PowerSwitch";
    }

    if(switchName != "") {
        SwitchComponent *switchComponent
            = getSwitchComponent(*(findRoot(this)), switchName);
        switchComponent->switchComponent(buttonName);
    }
}

MiniMap::DisplayMode getMode(const std::string &pName)
{
    if(pName=="MapViewNormal")
        return MiniMap::NORMAL;
    if(pName=="MapViewUB40")
        return MiniMap::UB40;
    if(pName=="MapViewPollution")
        return MiniMap::POLLUTION;
    if(pName=="MapViewTraffic")
        return MiniMap::TRAFFIC;
    if(pName=="MapViewCommodities")
        return MiniMap::COMMODITIES;
    if(pName=="MapViewFood")
        return MiniMap::STARVE;
    if(pName=="MapViewPower")
        return MiniMap::POWER;
    if(pName=="MapViewFire")
        return MiniMap::FIRE;
    if(pName=="MapViewSport")
        return MiniMap::CRICKET;
    if(pName=="MapViewHealth")
        return MiniMap::HEALTH;
    if(pName=="MapViewCoal")
        return MiniMap::COAL;

    return MiniMap::NORMAL;
}

std::string getNextButton(const std::string &pName)
{
    int i;
    for(i=0; mapViewButtons[i]; ++i)
        if(pName==mapViewButtons[i])
            break;
    assert(mapViewButtons[i]);

    i++;
    if(strlen(mapViewButtons[i])==0) {
        // end of row - go to beginning
        i--;
        // assuming that this is processed from front to back
        while(i>=0 && mapViewButtons[i] && strlen(mapViewButtons[i]))
        {   --i;}
        i++; // gone one too far
    }
#ifdef DEBUG
    assert(i>=0 && mapViewButtons[i] && strlen(mapViewButtons[i]));
#endif
    return mapViewButtons[i];
}

void MiniMap::mapViewChangeDisplayMode(DisplayMode newMode)
{
    Component *root = findRoot(this);
    std::string name;

    switch(newMode) {
      case NORMAL:      name="MapViewNormal"; break;
      case POLLUTION:   name="MapViewPollution"; break;
      case UB40:        name="MapViewUB40"; break;
      case STARVE:      name="MapViewFood"; break;
      case POWER:       name="MapViewPower"; break;
      case FIRE:        name="MapViewFire"; break;
      case CRICKET:     name="MapViewSport"; break;
      case HEALTH:      name="MapViewHealth"; break;
      case COAL:        name="MapViewCoal"; break;
      case TRAFFIC:     name="MapViewTraffic"; break;
      default: name="";
      //case COMMODITIES: name=//FIXME!
    }

    for(int b = 0; mapViewButtons[b] != 0; ++b)
    {
        if(strlen(mapViewButtons[b]))
        {
            CheckButton* button = getCheckButton(*root, mapViewButtons[b]);
            if(button->getName()==name)
                button->check();
            else
                button->uncheck();
        }
    }

    //FIXME there should be a way to actually use switch button
    if (newMode==mMode && mMode == TRAFFIC)
    {   newMode = COMMODITIES;}

    if(mMode == COAL) {
      if(!game->getWorld().coal_survey_done)
        new Dialog(*game, ASK_COAL_SURVEY);
    }

    mMode=newMode;
    //switchMapViewButton(name);
    switchView("MiniMap");
    game->getGameView().setMapMode( mMode );
    mFullRefresh=true;
}

void MiniMap::mapViewButtonClicked(CheckButton* button, int mousebutton)
{
    std::string name = button->getName();

    if(mousebutton == SDL_BUTTON_RIGHT ) {
    // switch button
        std::string pName=getNextButton(button->getName());

        if(pName=="MapViewNormal")
          game->showHelpWindow("msb-normal");
        else if (pName=="MapViewUB40")
          game->showHelpWindow("msb-ub40");
        else if (pName=="MapViewPollution")
          game->showHelpWindow("msb-pol");
        else if (pName=="MapViewFood")
          game->showHelpWindow("msb-starve");
        else if (pName=="MapViewPower")
          game->showHelpWindow("msb-power");
        else if (pName=="MapViewFire")
          game->showHelpWindow("msb-fire");
        else if (pName=="MapViewSport")
          game->showHelpWindow("msb-cricket");
        else if (pName=="MapViewHealth")
          game->showHelpWindow("msb-health");
        else if (pName=="MapViewCoal")
          game->showHelpWindow("msb-coal");
        else if (pName=="MapViewTraffic")
          game->showHelpWindow("msb-transport");

    return;
    }
    DisplayMode newMode=getMode(name);

    if(newMode==mMode)
    {
        // switch button
        name=getNextButton(name);
        newMode=getMode(name);
    }

    mapViewChangeDisplayMode(newMode);
}

void
MiniMap::speedButtonClicked(CheckButton* button, int)
{
    Component *root = findRoot(this);
    std::string name = button->getName();
    int i;
    for(i = 0; speedButtons[i] != 0; ++i) {
        if(name == speedButtons[i])
            break;
    }
#ifdef DEBUG
    assert(speedButtons[i] != 0);
#endif

    for(int b = 0; speedButtons[b] != 0; ++b) {
        if(b != i) {
            CheckButton* button = getCheckButton(*root, speedButtons[b]);
            button->uncheck();
        } else {
            button->check();
        }
    }

    switch(i)
    {
        case 0:
            setSimulationDelay(SIM_DELAY_PAUSE);
            break;
        case 1:
            setSimulationDelay(SIM_DELAY_SLOW);
            break;
        case 2:
            setSimulationDelay(SIM_DELAY_MED);
            break;
        case 3:
            setSimulationDelay(SIM_DELAY_FAST);
            break;
        default:
            assert(false);
            break;
    }
}

void
MiniMap::zoomInButtonClicked(Button* ) {
  game->getGameView().zoomIn();
}

void
MiniMap::zoomOutButtonClicked(Button* ) {
  game->getGameView().zoomOut();
}

void
MiniMap::scrollPageDownButtonClicked(Button* )
{
    scrollPageDown(true);
}

void
MiniMap::scrollPageUpButtonClicked(Button* )
{
    scrollPageDown(false);
}

void
MiniMap::scrollPageDown(bool down) {
  std::string viewname = getSwitchComponent(*(findRoot(this)), "MiniMapSwitch")->getActiveComponent()->getName();

  if(viewname == "MapMPS") {
    MpsMap& mps = game->getMpsMap();
    switch(mps.page) {
    case MpsMap::Page::INVENTORY:
      mps.page = MpsMap::Page::PRODUCTION;
      break;
    case MpsMap::Page::PRODUCTION:
      mps.page = MpsMap::Page::INVENTORY;
      break;
    case MpsMap::Page::GROUND:
      mps.page = MpsMap::Page::GROUND;
      break;
    }
    mps.refresh();
  }
  else if(viewname == "MiniMap") {
    toggleStuffID(down ? -1 : 1);
  }
  else if(viewname == "PBar") {
    switchView("PBar2nd");
  }
  else if(viewname == "PBar2nd") {
    switchView("PBar");
  }
  else if(viewname == "GlobalMPS") {
    MpsFinance& mps = game->getMpsFinance();
    switch(mps.page) {
    case MpsFinance::Page::CASH_FLOW:
      mps.page = down
        ? MpsFinance::Page::COSTS
        : MpsFinance::Page::POPULATION;
      break;
    case MpsFinance::Page::COSTS:
      mps.page = down
        ? MpsFinance::Page::POPULATION
        : MpsFinance::Page::CASH_FLOW;
      break;
    case MpsFinance::Page::POPULATION:
      mps.page = down
        ? MpsFinance::Page::CASH_FLOW
        : MpsFinance::Page::COSTS;
      break;
    }
    mps.refresh();
  }
#ifdef DEBUG
  else if(viewname == "EconomyGraph") {
    //has only one page for now
  }
  else {
      std::cout << "Unknown active component in MiniMapSwitch: " << viewname << std::endl;
      assert(false);
  }
#endif
}


Vector2
MiniMap::mapPointToVector(MapPoint p)
{
    return Vector2((p.x - left) * tilesize , (p.y -top) * tilesize);
}

void MiniMap::constrainPosition() {
    int minLeft = 1 - 1;
    int maxLeft = game->getWorld().map.len()-1 - ((int)(width/tilesize)-1);
    int minTop = 1 - 1;
    int maxTop = game->getWorld().map.len()-1 - ((int)(height/tilesize)-1);

    if(minLeft > maxLeft) {
        left = (minLeft + maxLeft) / 2;
    }
    else if(left < minLeft) {
        left = minLeft;
    }
    else if(left > maxLeft) {
        left = maxLeft;
    }

    if(minTop > maxTop) {
        top = (minTop + maxTop) / 2;
    }
    else if(top < minTop) {
        top = minTop;
    }
    else if(top > maxTop) {
        top = maxTop;
    }
}

/*
 *  Set the Corners of the GameView to show in Minimap
 */
void MiniMap::setGameViewCorners(
    const MapPoint& upperLeft, const MapPoint& lowerRight
) {
    this->upperLeft = upperLeft;
    this->lowerRight = lowerRight;
    left = (upperLeft.x + lowerRight.x) / 2 - (width / tilesize / 2);
    top  = (upperLeft.y + lowerRight.y) / 2 - (height / tilesize / 2);
    constrainPosition();
    mFullRefresh = true;
    setDirty();
}

void MiniMap::draw(Painter &painter) {
    Map& map = game->getWorld().map;
    attachButtons();
    int x, y;
    unsigned short size;

    // simple and bad implementation
    // FIXME: should be stored SDL_Surface and then blitted

    std::unique_ptr<Painter> mpainter
        (painter.createTexturePainter(mTexture.get()));
    Color white;
    white.parse( "white" );
    Rect2D miniRect( 0 , 0, width, height );
    Color mc = getColor( 0, 0 );
    if(mpainter.get() == 0)
    {
        // workaround - so that it works with GL, too, as long as there's no TexturePainter for this
        if( mFullRefresh )
        {
            painter.setFillColor( mc );
            painter.fillRectangle( miniRect );
            for(y=1;y<height/tilesize;y++)
            {
                for(x=1;x<width/tilesize;x++)
                {
                    if (map.is_visible(left+x, top+y)) /*left + x > 0 && top + y > 0 && left + x < world.len()-1 && top + y < world.len()-1)*/
                    {
                        if(map(left + x, top + y)->construction) {
                            size = map(left + x, top + y)->construction->constructionGroup->size;
                            mc = getColor(left + x,top + y);
                            painter.setFillColor(mc);
                            painter.fillRectangle(Rect2D((x)*tilesize,y*tilesize,(x+size)*tilesize+1,(y+size)*tilesize));
                        }
                        else if(!map(left + x, top + y)->reportingConstruction)
                        {
                            size = map(left + x, top + y)->getTileConstructionGroup()->size;
                            mc=getColor(left + x,top + y);
                            painter.setFillColor(mc);
                            painter.fillRectangle(Rect2D((x)*tilesize,(y)*tilesize,(x+size)*tilesize+1,(y+size)*tilesize));
                        }
                        if( mMode == COAL )
                        { //show coal under buildings, too
                            mc=getColor(left + x,top + y);
                            painter.setFillColor(mc);
                            painter.fillRectangle(Rect2D((x)*tilesize,(y)*tilesize,(x+1)*tilesize+1,(y+1)*tilesize));
                        }
                    }
                }
            }
        }

        //show current GameView
        painter.setClipRectangle( miniRect );
// FIXME:
//        painter.setLineColor( white );
//        painter.drawPolygon( 4, gameViewPoints );
        painter.clearClipRectangle();
        return;
    }
    if( mFullRefresh )
    {
        mpainter->setFillColor( mc );
        mpainter->fillRectangle( miniRect );

        for(y=1;y<height/tilesize;y++)
        {
            for(x=1;x<width/tilesize;x++)
            {
                if (map.is_visible(left+x, top+y))
                {
                    if(map(left + x, top + y)->construction) {
                        size = map(left + x, top + y)->construction->constructionGroup->size;
                        mc = getColor(left + x,top + y);
                        mpainter->setFillColor(mc);
                        mpainter->fillRectangle(Rect2D((x)*tilesize,(y)*tilesize,(x+size)*tilesize+1,(y+size)*tilesize));
                    }
                    else if(!map(left + x, top + y)->reportingConstruction) {
                        size = map(left + x, top + y)->getTileConstructionGroup()->size;
                        mc = getColor(left + x, top + y);
                        mpainter->setFillColor(mc);
                        mpainter->fillRectangle(Rect2D((x)*tilesize,(y)*tilesize,(x+size)*tilesize+1,(y+size)*tilesize));
                    }
                    if(mMode == COAL) { //show coal under buildings, too
                        mc=getColor(left + x,top + y);
                        mpainter->setFillColor(mc);
                        mpainter->fillRectangle(Rect2D((x)*tilesize,(y)*tilesize,(x+1)*tilesize+1,(y+1)*tilesize));
                    }
                }
            }
        }
    }

    //show current GameView
// FIXME:
//    mpainter->setLineColor( white );
//    mpainter->drawPolygon( 4, gameViewPoints );

    painter.drawTexture(mTexture.get(), Vector2(0, 0));
    mFullRefresh=false;
}

Color MiniMap::getColorNormal(int x, int y) const
{
    int mc = game->getWorld().map(x,y)->getConstructionGroup()->colour;

    int red = 0;
    int green = 0;
    int blue = 0;

    if( mc & 32 )
    {   red = 8 * ( mc & 31 );}
    if( mc & 64 )
    {   green = 8 * ( mc & 31 );}
    if( mc & 128 )
    {   blue = 8 * (mc & 31 );}

    return Color( red, green, blue );
}

Color MiniMap::getColor(int x,int y) const
{
    Map& map = game->getWorld().map;
    if(!map.is_inside( x, y))
    {
        x = 0;
        y = 0;
    }
    int xx = x;
    int yy = y;
    int conflags = 0;
    int mapflags = 0;
    unsigned short g = map(x,y)->getGroup();

    // only needed to look up xx,yy for old style flags
    if (map(x,y)->reportingConstruction)
    {
        xx = map(x,y)->reportingConstruction->x;
        yy = map(x,y)->reportingConstruction->y;
        conflags = map(x,y)->reportingConstruction->flags;
    }

    mapflags = map(xx,yy)->flags;
    switch(mMode)
    {
        case NORMAL:
#ifdef DEBUG
//#define DEBUG_ALTITUDE
#ifdef DEBUG_ALTITUDE
            if (map(xx,yy)->is_bare())
                {
                // show ground altitude
                int alt = map(xx,yy)->ground.altitude;
                if (alt > alt_min + 9 * alt_step)
                    return Color(255,255,255);          // white
                else if ( alt > alt_min + 8 * alt_step )
                    return Color(173,173,173);          // bright grey
                else if ( alt > alt_min + 7 * alt_step )
                    return Color(130,130,130);          // grey
                else if ( alt > alt_min + 6 * alt_step )
                    return Color(197,170,74);           // bright brown
                else if ( alt > alt_min + 5 * alt_step )
                    return Color(106,97,41);            // brown
                else if ( alt > alt_min + 4 * alt_step )
                    return Color(0,198,0);              // bright green
                else if ( alt > alt_min + 3 * alt_step )
                    return Color(16,125,8);             // green
                else if ( alt > alt_min + 2 * alt_step )
                    return Color(248,229,30);           // yellow
                else if ( alt > alt_min + 1 * alt_step )
                    return Color(8,93,255);             // bright blue
                else
                    return Color(0,0,0x9F);             // dark blue
            }
            else
#endif
#endif  //DEBUG
            {   return getColorNormal(xx,yy);}
        case POLLUTION:
        {
            short p = map(x,y)->pollution;
            float v = p/600.0;
            if(v < 0)
                v = 0;
            if(v > 1)
                v = 1;
            Color mc((int) (0xFF*v), (int) (0xFF*(1-v)), 0);
            mc = light(mc,brightness(getColorNormal(xx,yy)));
            return mc;
        }
        case FIRE:
            if( map(xx,yy)->getGroup() == GROUP_FIRE )
            {
                if( ! static_cast<Fire*>(map(xx,yy)->reportingConstruction)->smoking_days )
                {
                    return Color(0xFF,0,0); //still burning (red)
                } else  {
                    return Color(0xFF,0x99,0); //unbulldozable (orange)
                }
            }
/*
            //FIXME Does that group still exist?
            if( world(xx,yy)->getGroup() == GROUP_BURNT )
            {
                return Color(0xFF,0xFF,0); //(yellow)
            }
*/
            if( map(xx,yy)->reportingConstruction && map(xx,yy)->reportingConstruction->constructionGroup == &fireStationConstructionGroup)
            {
                return Color(0,0xFF,0); //green
            }
            //fall through
        case CRICKET:
            if( (map(xx,yy)->reportingConstruction && map(xx,yy)->reportingConstruction->constructionGroup == &cricketConstructionGroup)  && mMode==CRICKET )
            {
                return Color(0,0xFF,0); //green
            }
            //fall through
        case HEALTH:
            if( (map(xx,yy)->reportingConstruction && map(xx,yy)->reportingConstruction->constructionGroup == &healthCentreConstructionGroup)  && mMode==HEALTH )
            {
                return Color(0,0xFF,0); //green
            }
            if( ((mapflags & FLAG_FIRE_COVER) && mMode==FIRE) ||
                    ((mapflags & FLAG_CRICKET_COVER) && mMode==CRICKET) ||
                    ((mapflags & FLAG_HEALTH_COVER) && mMode==HEALTH))
            {
                Color mc(0,0xFF,0);
                mc = light(mc,brightness(getColorNormal(xx,yy)));
                return mc;
            }
            else
                return makeGrey(getColorNormal(xx,yy));
        case UB40: {
            /* Display residence with un/employed people (red / green) == too many people here */
            int job_level = map(xx,yy)->reportingConstruction?
            map(xx,yy)->reportingConstruction->tellstuff(STUFF_LABOR, -1):-1;
            if (job_level == -1) // Not a "jobby" place at all
            {
                return makeGrey(getColorNormal(xx,yy));
            }
            if ( map(xx,yy)->is_residence() )
            {
                if ( job_level > 95 * TRANSPORT_QUANTA / 100 )
                    return Color(0xFF,0,0);
                else if ( job_level > 90 * TRANSPORT_QUANTA / 100 )
                    return Color(0x7F,0,0);
                else
                   return makeGrey(getColorNormal(xx,yy));
                    //return Color(0,0xFF,0);
            }

            /* display buildings with unsatisfied requests for labor (yellow) == too few people here */

            else //not a residence
            {
                if ( job_level < 5 * TRANSPORT_QUANTA / 100)
                    return Color(0xFF,0xFF,0); // yellow
                else if ( job_level < 10 * TRANSPORT_QUANTA / 100)
                    return Color(0xFF,0x99,0); // orange
                else
                    return makeGrey(getColorNormal(xx,yy));
            }

        }
        case COAL: //don't use xx and yy for coal deposits
        {
            Color c(0x77,0,0);
            if(!game->getWorld().coal_survey_done) {
              return Color(0,0,0);
            }
            if(map(x,y)->coal_reserve==0)
                return makeGrey(getColorNormal(x,y));
            else if (map(x,y)->coal_reserve >= COAL_RESERVE_SIZE / 2)
                return Color(0,0xFF,0);
            else if (map(x,y)->coal_reserve < COAL_RESERVE_SIZE / 2)
                return Color(0xFF,0,0);

            return c;
        }
        case STARVE:
        {
            int food_level = map(xx,yy)->reportingConstruction?
            map(xx,yy)->reportingConstruction->tellstuff(STUFF_FOOD, -1):1;
            int water_level = map(xx,yy)->reportingConstruction?
            map(xx,yy)->reportingConstruction->tellstuff(STUFF_WATER, -1):1;
            int crit_level = water_level<food_level?water_level:food_level;
            //don't care about other eaters or drinkers
            if ( map(xx,yy)->is_residence() )
            {
                if ( crit_level < 5 * TRANSPORT_QUANTA / 100 )
                    return Color(0xFF,0,0);
                else if ( crit_level < 10 * TRANSPORT_QUANTA / 100 )
                    return Color(0x7F,0,0);
                else
                    return Color(0,0xFF,0);
            }
            else
            {   return makeGrey(getColorNormal(xx,yy));}
        }
        case POWER:
        {
            Color mc;
            /* default color = grey */
            //mc = Color(0x3F,0x3F,0x3F);
            mc = makeGrey(getColorNormal(xx,yy));
            int lovolt_level = map(xx,yy)->reportingConstruction?
            map(xx,yy)->reportingConstruction->tellstuff(STUFF_LOVOLT, -1):-1;
            int hivolt_level = map(xx,yy)->reportingConstruction?
            map(xx,yy)->reportingConstruction->tellstuff(STUFF_HIVOLT, -1):-1;
            if (lovolt_level > -1 || hivolt_level > -1)
            {
                /* not enough power */
                mc = Color(0xFF,0,0);
                /* kW powered */
                if (lovolt_level > 5 * TRANSPORT_QUANTA / 100)
                    mc = Color(0,0x7F,0);
                /* MW powered */
                if (hivolt_level > 5 * TRANSPORT_QUANTA / 100)
                        mc = Color(0,0xFF,0);
            }
            if (g == GROUP_POWER_LINE)
            {   mc = Color(0xFF,0xFF,0);} //yellow
            return mc;
        }
        case TRAFFIC:
        {
            if ( (conflags & FLAG_IS_TRANSPORT) || (g == GROUP_POWER_LINE))
            {
                float loc_lvl = -1;
                if (conflags & FLAG_IS_TRANSPORT)
                {
                    Transport *transport;
                    transport = static_cast<Transport *>(map(xx,yy)->reportingConstruction);
                    if(transport->constructionGroup->commodityRuleCount[stuff_ID].maxload)
                    {   loc_lvl = transport->trafficCount[stuff_ID];}
                }
                else if (g == GROUP_POWER_LINE)
                {
                    Powerline *powerline;
                    powerline = static_cast<Powerline *>(map(xx,yy)->reportingConstruction);
                    if(powerline->constructionGroup->commodityRuleCount[stuff_ID].maxload)
                    {   loc_lvl = powerline->trafficCount[stuff_ID];}
                }
                if (loc_lvl < 0)
                {   return makeGrey(getColorNormal(xx,yy));}
                loc_lvl = loc_lvl * TRANSPORT_RATE / TRANSPORT_QUANTA;

#ifdef DEBUG
                assert(!(loc_lvl > 0.928)); //should be limit from fluctuations
#endif
                loc_lvl = 2-2/(1+((loc_lvl>0.928)?1:loc_lvl/0.928));
                //more contrast for small loads
                Color mc((int) (0xFF*loc_lvl), (int) (0xFF*(1-loc_lvl)), 0);
                return mc;
            }
            else
            { //not a Transport, make bluish if in range of a markt
                if ((mapflags & FLAG_MARKET_COVER) &&
                    marketConstructionGroup.commodityRuleCount[stuff_ID].maxload)
                {
                    return makeBlue(getColorNormal(x,y));
                }
                return makeGrey(getColorNormal(xx,yy));
            }
        }
        case COMMODITIES:
        {
/*
            if (conflags & FLAG_IS_TRANSPORT)
                return makeGrey(getColorNormal(xx,yy));
*/
            float loc_lvl = map(xx,yy)->reportingConstruction?
                 map(xx,yy)->reportingConstruction->tellstuff(stuff_ID, -1):-1;
            float red, green, blue;

            if (loc_lvl < 0)
            {   return makeGrey(getColorNormal(xx,yy));}
            loc_lvl /= TRANSPORT_QUANTA;
            if (stuff_ID == STUFF_WASTE) //so far waste is the only bad commodity
            {   loc_lvl = 1 - loc_lvl;}
            red = 1 - 2.25 * loc_lvl;
            if (red < 0) red = 0;
            if (!map(xx,yy)->reportingConstruction->constructionGroup->commodityRuleCount[stuff_ID].take)
            {   red = 0;}
            blue = 1 - 2.25 * (1 - loc_lvl);
            if (blue < 0) blue = 0;
            if (!map(xx,yy)->reportingConstruction->constructionGroup->commodityRuleCount[stuff_ID].give)
            {   blue = 0;}
            green = 1 - red - blue;
            Color mc(static_cast<int>(0xFF*red), static_cast<int>(0xFF*green),static_cast<int>(0xFF*blue));
            return mc;
        }
        case MAX:
            std::cerr<<"Undefined MiniMap-Display-type!"<<std::endl;
    };
    // default coloor is a nasty looking pink
    return Color(0xFF,0,0xFF);
}

void MiniMap::event(const Event& event) {

    // int left, top;
    //
    // left = (upperLeft.x + lowerRight.x) / 2 - (width / tilesize / 2);
    // top  = (upperLeft.y + lowerRight.y) / 2 - (height / tilesize / 2);

    if(event.type == Event::MOUSEMOTION) {
        if(!event.inside) {
            inside = false;
            return;
        }
        if(!inside) { //mouse just enterd the minimap, show current mapmode
            game->getGameView().setMapMode(mMode);
            inside = true;
        }
        return;
    }


    if(!event.inside) {
        return;
    }
    if(event.type==Event::MOUSEBUTTONDOWN) {
        // get Tile, that was clicked
        MapPoint tile (
                (int) ((event.mousepos.x - border ) / tilesize + left),
                (int) ((event.mousepos.y - border ) / tilesize) + top);

        if(event.mousebutton == SDL_BUTTON_LEFT )
        {   game->getGameView().show(tile);} // move main-map
/*
        if(event.mousebutton == SDL_BUTTON_RIGHT ) {

        }
*/
    } else if (event.type == Event::MOUSEWHEEL) {
        if (event.scrolly > 0) {
            scrollPageDown(false);
        }
        if (event.scrolly < 0) {
            scrollPageDown(true);
        }
    }
/*
    else if (event.type == Event::KEYDOWN)
    {
        //moved 'm' and 'n' for pagescrolling to Gameview
    }
*/
}

IMPLEMENT_COMPONENT_FACTORY(MiniMap)


/** @file lincity-ng/MiniMap.cpp */
