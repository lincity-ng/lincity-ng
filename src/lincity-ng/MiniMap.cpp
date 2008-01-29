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

#include "MiniMap.hpp"
#include "GameView.hpp"
#include "MainLincity.hpp"

#include "gui/Painter.hpp"
#include "gui/Button.hpp"
#include "gui/Rect2D.hpp"
#include "gui/ComponentFactory.hpp"
#include "gui/XmlReader.hpp"
#include "gui/Event.hpp"
#include "gui/SwitchComponent.hpp"
#include "gui_interface/mps.h"

#include "lincity/lin-city.h"
#include "lincity/engglobs.h"
#include "lincity/lctypes.h"

#include "gui/callback/Callback.hpp"

#include "gui_interface/shared_globals.h"
#include "gui_interface/screen_interface.h"

#include <set>
#include <iostream>

#include "Debug.hpp"
#include "Util.hpp"
#include "Mps.hpp"
#include "CheckButton.hpp"
#include "Dialog.hpp"
#include "Game.hpp"
#include "HelpWindow.hpp"

extern int get_power(int x, int y, int power, int block_industry);

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
    : mMode(NORMAL), tilesize(2), border(0), mTexture(0)
{   
    assert( miniMapPtr == 0 );
    miniMapPtr = this;
    gameViewPoints[ 0 ].x = 0;
    gameViewPoints[ 0 ].y = 0;
    gameViewPoints[ 1 ].x = 0;
    gameViewPoints[ 1 ].y = 0;
    gameViewPoints[ 2 ].x = 0;
    gameViewPoints[ 2 ].y = 0;
    gameViewPoints[ 3 ].x = 0;
    gameViewPoints[ 3 ].y = 0;
}

MiniMap::~MiniMap()
{
    if(miniMapPtr == this)
        miniMapPtr = 0;
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

    mpsXOld = mps_x;
    mpsYOld = mps_y;
    mpsStyleOld = mps_style;

    //switchView("MiniMap");
}

Component *MiniMap::findRoot(Component *c)
{
    while(c->getParent())
        return findRoot(c->getParent());

    return c;
}

void MiniMap::attachButtons()
{
    if(alreadyAttached)
        return;
    alreadyAttached=true;
  
    Component *root=findRoot(this);
  
    for(int i = 0; mapViewButtons[i] != 0; ++i) {
        if(strlen(mapViewButtons[i])) {
            CheckButton* b = getCheckButton(*root, mapViewButtons[i]);
            if(i == 0)
                b->check();
            b->clicked.connect(makeCallback(*this, &MiniMap::mapViewButtonClicked));
        }
    }

    for(int i = 0; speedButtons[i] != 0; ++i) {
        CheckButton* b = getCheckButton(*root, speedButtons[i]);
	setLincitySpeed(SLOW_TIME_FOR_YEAR);
        if(i == 1)
            b->check();
        b->clicked.connect(makeCallback(*this, &MiniMap::speedButtonClicked));
    }

    Button* zoomInButton = getButton(*root, "ZoomInButton");
    zoomInButton->clicked.connect(makeCallback(*this, &MiniMap::zoomInButtonClicked));
    Button* zoomOutButton = getButton(*root, "ZoomOutButton");
    zoomOutButton->clicked.connect(makeCallback(*this, &MiniMap::zoomOutButtonClicked));

    CheckButton* switchMinimapButton = getCheckButton(*root, "SwitchMiniMap");
    switchMinimapButton->clicked.connect(
            makeCallback(*this, &MiniMap::switchButton));
    switchButtons.push_back(switchMinimapButton);
    
    CheckButton* switchPBarButton = getCheckButton(*root, "SwitchPBar");
    switchPBarButton->clicked.connect(
            makeCallback(*this, &MiniMap::switchButton));
    switchButtons.push_back(switchPBarButton);
   
    CheckButton* switchButton = getCheckButton(*root, "SwitchGlobalMPS");
    switchButton->clicked.connect(
            makeCallback(*this, &MiniMap::switchButton));
    switchButtons.push_back(switchButton);
    
    switchButton = getCheckButton(*root, "SwitchEconomyGraph");
    switchButton->clicked.connect(
            makeCallback(*this, &MiniMap::switchButton));
    switchButtons.push_back(switchButton);
}

void
MiniMap::switchButton(CheckButton* button, int mousebutton)
{
    if( !alreadyAttached ){
        return;
    }
    if(mousebutton == SDL_BUTTON_RIGHT ) {
            getGame()->showHelpWindow( "dialogs" );
    }

    if(button->getName() == "SwitchGlobalMPS") {
        //cycle through global styles
        mps_global_style++;
        if( mps_global_style >= MPS_GLOBAL_STYLES ){
            mps_global_style = 0;
        }
        mps_set(mps_x, mps_y, MPS_GLOBAL);
    } else if(button->getName() == "SwitchMapMPS") {
        mps_set(mps_x, mps_y, MPS_MAP);
    }
    
    // remove "Switch" prefix
    std::string switchname = std::string(button->getName(),
            6, button->getName().size()-1);
    switchView(switchname);
}

void
MiniMap::switchView(const std::string& viewname)
{
    SwitchComponent* switchComponent 
        = getSwitchComponent(*(findRoot(this)), "MiniMapSwitch");

    switchComponent->switchComponent(viewname);

    std::string buttonname = "Switch";
    buttonname += viewname;
    for(std::vector<CheckButton*>::iterator i = switchButtons.begin();
            i != switchButtons.end(); ++i) {
        CheckButton* cbutton = *i;
        if(cbutton->getName() == buttonname) {
            cbutton->check();
        } else {
            cbutton->uncheck();
        }
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


void MiniMap::hideMpsEnv(){
    if( mps_style != MPS_ENV ){
        return;
    }
    //restore saved stats
    mps_x = mpsXOld;
    mps_y = mpsYOld;
    mps_style = mpsStyleOld;
    mps_set( mps_x, mps_y, mps_style );
    switchView(lastTabName);
}

void MiniMap::showMpsEnv( MapPoint tile ){
    if(mps_style != MPS_ENV) {
        SwitchComponent* switchComponent 
            = getSwitchComponent(*(findRoot(this)), "MiniMapSwitch");
                
        //save old minimap and MPS setting
        lastTabName = switchComponent->getActiveComponent()->getName(); 
        mpsXOld = mps_x;
        mpsYOld = mps_y;
        mpsStyleOld = mps_style;
    }
    mps_set( tile.x, tile.y, MPS_ENV );//show basic info
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
            i--;
        i++; // gone one too far
    }
    assert(i>=0 && mapViewButtons[i] && strlen(mapViewButtons[i]));
    
    return mapViewButtons[i];
}

void MiniMap::mapViewButtonClicked(CheckButton* button, int mousebutton)
{
    Component *root = findRoot(this);
    std::string name = button->getName();
    
    if(mousebutton == SDL_BUTTON_RIGHT ) {
	// switch button
        std::string pName=getNextButton(button->getName());

        if(pName=="MapViewNormal")
            getGame()->showHelpWindow( "msb-normal" );
        else if (pName=="MapViewUB40")
	    getGame()->showHelpWindow( "msb-ub40" );
        else if (pName=="MapViewPollution")
	    getGame()->showHelpWindow( "msb-pol" );
        else if (pName=="MapViewFood")
	    getGame()->showHelpWindow( "msb-starve" );
        else if (pName=="MapViewPower")
	    getGame()->showHelpWindow( "msb-power" );
        else if (pName=="MapViewFire")
	    getGame()->showHelpWindow( "msb-fire" );
        else if (pName=="MapViewSport")
	    getGame()->showHelpWindow( "msb-cricket" );
        else if (pName=="MapViewHealth")
	    getGame()->showHelpWindow( "msb-health" );
        else if (pName=="MapViewCoal")
	    getGame()->showHelpWindow( "msb-coal" );
        else if (pName=="MapViewTraffic")
	    getGame()->showHelpWindow( "msb-transport" );
    
    return;
    }
     
    DisplayMode newMode=getMode(button->getName());
    if(newMode==mMode) {
        // switch button
        name=getNextButton(button->getName());
        mMode=getMode(name);
    } else {
        mMode=newMode;
    }
    
    if(mMode==COAL) {
        if(( coal_survey_done == 0 ) && ( !blockingDialogIsOpen ))
            new Dialog( ASK_COAL_SURVEY ); 
    }
    
    for(int b = 0; mapViewButtons[b] != 0; ++b) {
        if(strlen(mapViewButtons[b])) {
            CheckButton* button = getCheckButton(*root, mapViewButtons[b]);
            if(button->getName()==name)
                button->check();
            else
                button->uncheck();
        }
    }

    switchMapViewButton(name);

    switchView("MiniMap");
    getGameView()->setMapMode( mMode ); 
    mFullRefresh=true;
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
    assert(speedButtons[i] != 0);

    for(int b = 0; speedButtons[b] != 0; ++b) {
        if(b != i) {
            CheckButton* button = getCheckButton(*root, speedButtons[b]);
            button->uncheck();
        } else {
            button->check();
        }
    }

    switch(i) {
        case 0:
            setLincitySpeed(0);
            break;
        case 1:
            setLincitySpeed(SLOW_TIME_FOR_YEAR);
            break;
        case 2:
            setLincitySpeed(MED_TIME_FOR_YEAR);
            break;
        case 3:
            setLincitySpeed(fast_time_for_year);
            break;
        default:
            assert(false);
            break;
    }
}

void
MiniMap::zoomInButtonClicked(Button* )
{
    getGameView()->zoomIn();
}

void
MiniMap::zoomOutButtonClicked(Button* )
{
    getGameView()->zoomOut();
}

Vector2
MiniMap::mapPointToVector(MapPoint p)
{
    return Vector2(p.x * tilesize, p.y * tilesize);
}

/*
 *  Set the Corners of the GameView to show in Minimap
 */
void MiniMap::setGameViewCorners(const MapPoint& upperLeft,
        const MapPoint& upperRight, const MapPoint& lowerRight,
        const MapPoint& lowerLeft )
{
    gameViewPoints[ 0 ] = mapPointToVector(upperLeft);
    gameViewPoints[ 1 ] = mapPointToVector(upperRight);
    gameViewPoints[ 2 ] = mapPointToVector(lowerRight);
    gameViewPoints[ 3 ] = mapPointToVector(lowerLeft);
    mFullRefresh = true;
    setDirty();
}

void MiniMap::draw(Painter &painter)
{
    attachButtons();
    int x, y;
    short typ, grp;

    // simple and bad implementation
    // FIXME: should be stored SDL_Surface and then blitted
    // SDL_Surface should be updated, only if needed

    std::auto_ptr<Painter> mpainter 
        (painter.createTexturePainter(mTexture.get()));
    Color white;
    white.parse( "white" );
    Rect2D miniRect( 0 , 0, WORLD_SIDE_LEN*tilesize, WORLD_SIDE_LEN*tilesize );
    Color mc = getColor( 0, 0 ); 
    if(mpainter.get() == 0) {
        // workaround - so that it works with GL, too, as long as there's no TexturePainter for this
        for(y=1;y<WORLD_SIDE_LEN-1 && y<height/tilesize;y++)
            for(x=1;x<WORLD_SIDE_LEN-1 && x<width/tilesize;x++) {
                typ = MP_TYPE(x,y);
                if( mFullRefresh || typ != mappointoldtype[x][y] ) {
                    mappointoldtype[x][y] = typ;
                    if( typ != CST_USED ){
                        grp = get_group_of_type(typ);
                        mc=getColor(x,y);
                        painter.setFillColor(mc);
                        painter.fillRectangle(Rect2D(x*tilesize,y*tilesize,(x+main_groups[grp].size)*tilesize+1,(y+main_groups[grp].size)*tilesize));
                    } else if( mMode == COAL ) { //show coal under buildings, too
                        mc=getColor(x,y);
                        painter.setFillColor(mc);
                        painter.fillRectangle(Rect2D(x*tilesize,y*tilesize,(x+1)*tilesize+1,(y+1)*tilesize));
                    }
                }
            }
        
        //show current GameView
        painter.setClipRectangle( miniRect ); 
        painter.setLineColor( white );
        painter.drawPolygon( 4, gameViewPoints );    
        painter.clearClipRectangle();
        return;
    }
    if( mFullRefresh ){
        mpainter->setFillColor( mc );
        mpainter->fillRectangle( miniRect );
    }
    for(y=1;y<WORLD_SIDE_LEN-1 && y<height/tilesize;y++) {
        for(x=1;x<WORLD_SIDE_LEN-1 && x<width/tilesize;x++) {
            typ = MP_TYPE(x,y);
            if ( mFullRefresh || typ != mappointoldtype[x][y] ) {
                mappointoldtype[x][y] = typ;
                if( typ != CST_USED ){
                    grp = get_group_of_type(typ);
                    mc=getColor(x,y);
                    mpainter->setFillColor(mc);
                    mpainter->fillRectangle(Rect2D(x*tilesize,y*tilesize,(x+main_groups[grp].size)*tilesize+1,(y+main_groups[grp].size)*tilesize));
                }
            }
        }
    }
    
    //show current GameView
    mpainter->setLineColor( white );
    mpainter->drawPolygon( 4, gameViewPoints );    

    painter.drawTexture(mTexture.get(), Vector2(0, 0));

    mFullRefresh=false;
}

Color MiniMap::getColorNormal(int x, int y) const
{
    int xx,yy;
  
    xx=x;
    yy=y;
    if (MP_TYPE(x,y) == CST_USED) {
        xx = MP_INFO(x,y).int_1;
        yy = MP_INFO(x,y).int_2;
    }

    int mc = main_groups[MP_GROUP(xx,yy)].colour;
    int red = 0;
    int green = 0;
    int blue = 0;

    if( mc & 32 ) 
        red = 8 * ( mc & 31 );
    if( mc & 64 )
        green = 8 * ( mc & 31 );
    if( mc & 128 )
        blue = 8 * (mc & 31 );
    
    return Color( red, green, blue ); 
}

Color MiniMap::getColor(int x,int y) const
{
    int xx = x;
    int yy = y;
  
    if (MP_TYPE(x,y) == CST_USED) {
        xx = MP_INFO(x,y).int_1;
        yy = MP_INFO(x,y).int_2;
    }
    /* FIXME: ugly coding: since here we should use xx, yy and not x,y */

    int flags=MP_INFO(xx,yy).flags;

    switch(mMode) {
        case NORMAL:
            return getColorNormal(x,y);
        case POLLUTION: {
            short p=MP_POL(x,y);
            float v=p/600.0;
            if(v<0)
                v=0;
            if(v>1)
                v=1;
            Color mc((int) (0xFF*v), (int) (0xFF*(1-v)), 0);
            mc=light(mc,brightness(getColorNormal(x,y)));
            return mc;
        }
        case FIRE:
            if( MP_GROUP( x, y ) == GROUP_FIRE ){
                if( MP_INFO(x,y).int_2 < FIRE_LENGTH ){
                    return Color(0xFF,0,0); //still burning (red)
                } else  {
                    return Color(0xFF,0x99,0); //unbulldozable (orange)
                }
            }
            if( MP_GROUP( x, y ) == GROUP_BURNT ){
                return Color(0xFF,0xFF,0); //(yellow)
            }
            if( MP_GROUP( x, y ) == GROUP_FIRESTATION ){
                return Color(0,0xFF,0); //green
            }
            //fall through
        case CRICKET:
            if( (MP_GROUP( x, y ) == GROUP_CRICKET)  && mMode==CRICKET ){
                return Color(0,0xFF,0); //green
            }
            //fall through
        case HEALTH:
            if( (MP_GROUP( x, y ) == GROUP_HEALTH)  && mMode==HEALTH ){
                return Color(0,0xFF,0); //green
            }
            if( ((flags&FLAG_FIRE_COVER) && mMode==FIRE) ||
                    ((flags&FLAG_CRICKET_COVER) && mMode==CRICKET) ||
                    ((flags&FLAG_HEALTH_COVER) && mMode==HEALTH))
            {
                Color mc(0,0xFF,0);
                mc=light(mc,brightness(getColorNormal(x,y)));
                return mc;
            }
            else
                return makeGrey(getColorNormal(x,y));
        case UB40: {
//#define DEBUG_ALTITUDE
#ifdef DEBUG_ALTITUDE
            // show ground altitude :-)
            int alt=ground[x][y].altitude;
            if (alt > 4500)
                    return Color(0xFF,0,0); //red
            else if ( alt> 4000 ) 
                    return Color(0xFF,0x99,0); //orange
	    else if ( alt > 3500 )
	                return Color(0xFF,0xFF,0); //yellow
	    else if ( alt > 3000 )
	                return Color(0,0xFF,0); //green
            else if ( alt > 2500)
                return Color(0,0,0xFF); // blue
            else if (alt > 2000)
                    return Color(0xFF,0,0); //red
            else if ( alt> 1500 ) 
                    return Color(0xFF,0x99,0); //orange
	    else if ( alt > 1000 )
	                return Color(0xFF,0xFF,0); //yellow
	    else if ( alt > 500 )
	                return Color(0,0xFF,0); //green
            else
                return Color(0,0,0xFF); // blue
#endif


            if (MP_GROUP_IS_RESIDENCE(xx,yy)) {
                if (MP_INFO(xx,yy).int_1 < -20)
                    return Color(0xFF,0,0);
                else if (MP_INFO(xx,yy).int_1 < 10)
                    return Color(0x7F,0,0);
                else
                    return Color(0,0xFF,0);
            } else {
                return makeGrey(getColorNormal(x,y));
            }
        }
        case COAL: {
            Color c(0x77,0,0);
            if( coal_survey_done == 0 ) {
                return Color(0,0,0);
            }
            if(MP_INFO(x,y).coal_reserve==0)
                return makeGrey(getColorNormal(x,y));
            else if (MP_INFO(x,y).coal_reserve >= COAL_RESERVE_SIZE / 2)
                return Color(0,0xFF,0);
            else if (MP_INFO(x,y).coal_reserve < COAL_RESERVE_SIZE / 2)
                return Color(0xFF,0,0);
            
            return c;
        }
        case STARVE: {
            if (MP_GROUP_IS_RESIDENCE(xx,yy)) {
                if ((total_time - MP_INFO(xx,yy).int_2) < 20)
                    return Color(0xFF,0,0);
                else if ((total_time - MP_INFO(xx,yy).int_2) < 100)
                    return Color(0x7F,0,0);
                else
                    return Color(0,0xFF,0);
            } else {
                    if (use_waterwell) {
                        if ((flags & FLAG_WATERWELL_COVER) != 0)
                                return makeBlue(getColorNormal(x,y));
                    }
                    return makeGrey(getColorNormal(x,y));
            }
        }
        case POWER: {
            Color mc;
            /* default color = grey */
            mc=Color(0xFF,0xFF,0xFF);
            if (MP_INFO(xx, yy).flags & FLAG_ASKED_FOR_POWER) {
                if ( (MP_INFO(xx, yy).flags & FLAG_GOT_POWER) != 0) {
                    /* Windmill powered */
                    if (get_power (xx, yy, 0, 0) != 0)
                        mc=Color(0,0x7F,0);
                    /* Normal powered */
                    if (get_power (xx, yy, 0, 1) != 0)
                        mc=Color(0,0xFF,0);
                } else {
                    /* did not get power */
                    /* !!! don't call get power here or the flags will be set :-) */
                        mc=Color(0xFF,0,0); // (red)
                }
            }
            if (MP_GROUP(xx,yy) == GROUP_POWER_LINE)
                mc=Color(0xFF,0xFF,0); //yellow

            mc=light( mc,(0xAA + brightness(getColorNormal(xx,yy)))/2 );
            return mc;
        }
        case TRAFFIC: {
            if ( MP_INFO(x,y).flags & FLAG_IS_TRANSPORT ) {
                float max;
                float nextValue;
                if( MP_GROUP(x,y) == GROUP_ROAD){
                    max = MP_INFO(x,y).int_1 * 100.0 / MAX_FOOD_ON_ROAD;
                    nextValue = MP_INFO(x,y).int_2 * 100.0 / MAX_JOBS_ON_ROAD;
                    if( nextValue > max ){ max = nextValue; }
                    nextValue = MP_INFO(x,y).int_3 * 100.0 / MAX_COAL_ON_ROAD;
                    if( nextValue > max ){ max = nextValue; }
                    nextValue = MP_INFO(x,y).int_4 * 100.0 / MAX_GOODS_ON_ROAD;
                    if( nextValue > max ){ max = nextValue; }
                    nextValue = MP_INFO(x,y).int_5 * 100.0 / MAX_ORE_ON_ROAD;
                    if( nextValue > max ){ max = nextValue; }
                    nextValue = MP_INFO(x,y).int_6 * 100.0 / MAX_STEEL_ON_ROAD;
                    if( nextValue > max ){ max = nextValue; }
                    nextValue = MP_INFO(x,y).int_7 * 100.0 / MAX_WASTE_ON_ROAD;
                    if( nextValue > max ){ max = nextValue; }
                } else if( MP_GROUP(x,y) == GROUP_TRACK ) {
                    max = MP_INFO(x,y).int_1 * 100.0 / MAX_FOOD_ON_TRACK;
                    nextValue = MP_INFO(x,y).int_2 * 100.0 / MAX_JOBS_ON_TRACK;
                    if( nextValue > max ){ max = nextValue; }
                    nextValue = MP_INFO(x,y).int_3 * 100.0 / MAX_COAL_ON_TRACK;
                    if( nextValue > max ){ max = nextValue; }
                    nextValue = MP_INFO(x,y).int_4 * 100.0 / MAX_GOODS_ON_TRACK;
                    if( nextValue > max ){ max = nextValue; }
                    nextValue = MP_INFO(x,y).int_5 * 100.0 / MAX_ORE_ON_TRACK;
                    if( nextValue > max ){ max = nextValue; }
                    nextValue = MP_INFO(x,y).int_6 * 100.0 / MAX_STEEL_ON_TRACK;
                    if( nextValue > max ){ max = nextValue; }
                    nextValue = MP_INFO(x,y).int_7 * 100.0 / MAX_WASTE_ON_TRACK;
                    if( nextValue > max ){ max = nextValue; }
                } else {
                    max = MP_INFO(x,y).int_1 * 100.0 / MAX_FOOD_ON_RAIL;
                    nextValue = MP_INFO(x,y).int_2 * 100.0 / MAX_JOBS_ON_RAIL;
                    if( nextValue > max ){ max = nextValue; }
                    nextValue = MP_INFO(x,y).int_3 * 100.0 / MAX_COAL_ON_RAIL;
                    if( nextValue > max ){ max = nextValue; }
                    nextValue = MP_INFO(x,y).int_4 * 100.0 / MAX_GOODS_ON_RAIL;
                    if( nextValue > max ){ max = nextValue; }
                    nextValue = MP_INFO(x,y).int_5 * 100.0 / MAX_ORE_ON_RAIL;
                    if( nextValue > max ){ max = nextValue; }
                    nextValue = MP_INFO(x,y).int_6 * 100.0 / MAX_STEEL_ON_RAIL;
                    if( nextValue > max ){ max = nextValue; }
                    nextValue = MP_INFO(x,y).int_7 * 100.0 / MAX_WASTE_ON_RAIL;
                    if( nextValue > max ){ max = nextValue; }
                }
                if( max > 99 )          //red
                    return Color(0xFF,0,0);
                else if ( max > 85 )    //orange
                    return Color(0xFF,0x99,0); 
	            else if ( max > 50 )    //yellow
	                return Color(0xFF,0xFF,0); 
	            else                    //green
	                return Color(0,0xFF,0); 
	        } else { //not a Transport, make bluish if in range of a markt
                if (numof_markets > 0) {
                    for ( int q = 0; q < numof_markets; q++) {
	                    if ( (abs (marketx[q] - xx) < MARKET_RANGE)
                             && (abs (markety[q] - yy) < MARKET_RANGE) )
	                    return makeBlue(getColorNormal(x,y));
                    }
                }
	            return makeGrey(getColorNormal(x,y));
            }
        }
        case MAX:
            std::cerr<<"Undefined MiniMap-Display-type!"<<std::endl;
    };
    
    return Color(0xFF,0,0xFF);
}

void MiniMap::event(const Event& event) {
    if(event.type == Event::MOUSEMOTION) {
        if(!event.inside) {
            inside = false;
            return;
        }
        if(!inside) { //mouse just enterd the minimap, show current mapmode
            getGameView()->setMapMode( mMode ); 
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
                (int) ((event.mousepos.x - border ) / tilesize),
                (int) ((event.mousepos.y - border ) / tilesize));
        
        if(event.mousebutton == SDL_BUTTON_LEFT ) {
            getGameView()->show(tile); // move main-map
        }
        if(event.mousebutton == SDL_BUTTON_WHEELUP ) { 
            getGameView()->zoomIn();
        }
        if(event.mousebutton == SDL_BUTTON_WHEELDOWN ){
            getGameView()->zoomOut();
        }
    }
}

IMPLEMENT_COMPONENT_FACTORY(MiniMap);

