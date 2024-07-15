/*
Copyright (C) 2005 Wolfgang Becker <uafr@gmx.de>

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

#include "GameView.hpp"

#include <SDL.h>                           // for SDL_BUTTON_LEFT, SDL_BUTTO...
#include <SDL_image.h>                     // for IMG_Load_RW
#include <assert.h>                        // for assert
#include <physfs.h>                        // for PHYSFS_exists
#include <stdio.h>                         // for size_t, sscanf, NULL
#include <string.h>                        // for strcmp
#include <cmath>                           // for sqrt, fabs, floor, fabsf
#include <exception>                       // for exception
#include <iostream>                        // for basic_ostream, operator<<
#include <iterator>                        // for advance
#include <list>                            // for _List_iterator, list, oper...
#include <map>                             // for _Rb_tree_iterator, map
#include <sstream>                         // for basic_stringstream, basic_...
#include <stdexcept>                       // for runtime_error
#include <utility>                         // for pair
#include <vector>                          // for vector

#include "Config.hpp"                      // for getConfig, Config
#include "Dialog.hpp"                      // for blockingDialogIsOpen
#include "MapEdit.hpp"                     // for check_bulldoze_area, editMap
#include "MiniMap.hpp"                     // for MiniMap, getMiniMap
#include "Mps.hpp"                         // for mps_x, mps_y
#include "PhysfsStream/PhysfsSDL.hpp"      // for getPhysfsSDLRWops
#include "Util.hpp"                        // for getButton, getParagraph
#include "gui/Button.hpp"                  // for Button
#include "gui/Color.hpp"                   // for Color
#include "gui/ComponentFactory.hpp"        // for IMPLEMENT_COMPONENT_FACTORY
#include "gui/Desktop.hpp"                 // for Desktop
#include "gui/Event.hpp"                   // for Event
#include "gui/Painter.hpp"                 // for Painter
#include "gui/Paragraph.hpp"               // for Paragraph
#include "gui/Rect2D.hpp"                  // for Rect2D
#include "gui/Texture.hpp"                 // for Texture
#include "gui/TextureManager.hpp"          // for TextureManager, texture_ma...
#include "gui/XmlReader.hpp"               // for XmlReader
#include "gui/callback/Callback.hpp"       // for makeCallback, Callback
#include "gui/callback/Signal.hpp"         // for Signal
#include "gui_interface/mps.h"             // for mps_set, MPS_MAP, mps_refresh
#include "gui_interface/shared_globals.h"  // for main_screen_originx, main_...
#include "libxml/xmlreader.h"              // for XML_READER_TYPE_ELEMENT
#include "lincity/UserOperation.h"         // for UserOperation
#include "lincity/all_buildings.h"         // for TileConstructionGroup, GRO...
#include "lincity/commodities.hpp"         // for commodityNames
#include "lincity/engglobs.h"              // for userOperation, world, alt_...
#include "lincity/engine.h"                // for desert_water_frontiers
#include "lincity/groups.h"                // for GROUP_DESERT, GROUP_WATER
#include "lincity/lin-city.h"              // for FLAG_POWER_CABLES_0, FLAG_...
#include "lincity/lintypes.h"              // for ConstructionGroup, Constru...
#include "lincity/transport.h"             // for connect_transport, BRIDGE_...
#include "lincity/world.h"                 // for World, MapTile, Ground
#include "tinygettext/gettext.hpp"         // for _, dictionaryManager
#include "tinygettext/tinygettext.hpp"     // for Dictionary, DictionaryManager


const int scale3d = 128; // guestimate value for good looking 3d view;

const float GameView::defaultTileWidth = 128;
const float GameView::defaultTileHeight = 64;
const float GameView::defaultZoom = 1.0;    // fastest drawing

GameView* gameViewPtr = 0;

GameView* getGameView() {
  if(!gameViewPtr) throw std::runtime_error("gameViewPtr is not initialized");
  return gameViewPtr;
}


GameView::GameView()
{
    assert(gameViewPtr == 0);
    gameViewPtr = this;
    loaderThread = 0;
    keyScrollState = SCROLL_NONE;
    mouseScrollState = SCROLL_NONE;
    remaining_images = 0;
    textures_ready = false;
}

GameView::~GameView()
{
    stopThread = true;
    SDL_WaitThread( loaderThread, NULL );
    if(gameViewPtr == this)
    {   gameViewPtr = 0;}
}

//Static function to use with SDL_CreateThread
int GameView::gameViewThread( void* data )
{
    GameView* gv = (GameView*) data;
    gv->preReadImages();
    gv->textures_ready = true;
    //keep thread alive as long as there are SDL_Surfaces
    while(!gv->stopThread && gv->remaining_images!=0)
    {   SDL_Delay(100);}
    return 0;
}

void GameView::parse(XmlReader& reader)
{
    //Read from config
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        //check if Attribute handled by parent
        if(parseAttribute(attribute, value)) {
            continue;
        } else {
            std::cerr << "GameView::parse# Skipping unknown attribute '" << attribute << "'.\n";
        }
    }
    // no more elements to parse
    blankGraphicsInfo.texture = readTexture( "blank.png" );
    blankGraphicsInfo.x = blankGraphicsInfo.texture->getWidth() / 2;
    blankGraphicsInfo.y = blankGraphicsInfo.texture->getHeight();

    stopThread = false;
    loaderThread = SDL_CreateThread( gameViewThread, "Loader", this );

    //GameView is resizable
    setFlags(FLAG_RESIZABLE);

    //start at location from savegame
    zoom = defaultZoom;
    tileWidth = defaultTileWidth * zoom;
    tileHeight = defaultTileHeight * zoom;
    virtualScreenWidth = tileWidth * world.len();
    virtualScreenHeight = tileHeight * world.len();
    readOrigin( false );

    mouseInGameView = false;
    dragging = false;
    leftButtonDown = false;
    roadDragging = false;
    ctrDrag = false;
    areaBulldoze = false;
    startRoad = MapPoint(0, 0);
    rightButtonDown = false;
    tileUnderMouse = MapPoint(0, 0);
    dragStart = Vector2(0, 0);
    hideHigh = false;
    showTerrainHeight = false;
    cursorSize = 0;
    mpsEnvOnQuery = false;

    mapOverlay = overlayNone;
    mapMode = MiniMap::NORMAL;
    buttonsConnected = false;
    lastStatusMessage = "";
}

/*
 * Connect to GUI-Buttons.
 */
void GameView::connectButtons(){
    if( buttonsConnected ){
        return;
    }
    buttonsConnected = true;
    Component* root = this;
    while( root->getParent() ){
        root = root->getParent();
    }
    Button* button = getButton( *root, "hideHighBuildings" );
    button->clicked.connect( makeCallback(*this, &GameView::buttonClicked ) );

    button = getButton( *root, "showTerrainHeight" );
    button->clicked.connect( makeCallback(*this, &GameView::buttonClicked ) );

    button = getButton( *root, "mapOverlay" );
    button->clicked.connect( makeCallback(*this, &GameView::buttonClicked ) );
}

/*
 * Handle Clicks on Button.
 */
void GameView::buttonClicked( Button* button ){
    std::string name = button->getName();
    if( name == "hideHighBuildings" ){
        hideHigh = !hideHigh;
        requestRedraw();
        return;
    }
    if( name == "showTerrainHeight" ){
        if( alt_step != 0 ){
            showTerrainHeight = !showTerrainHeight;
        } else { // map is completely flat
            showTerrainHeight = false;
        }
        requestRedraw();
        return;
    }
    if( name == "mapOverlay" ){
        mapOverlay = (mapOverlay + 1) % (overlayMAX + 1);
        requestRedraw();
        return;
    }
    std::cerr << "GameView::buttonClicked# Unhandled Button '" << name <<"',\n";
}
int GameView::gameAreaMax()
{
    return world.len() -2;
}

/*
 * size in Tiles of marking under Cursor
 * atm 0 is an outlined Version of size 1.
 */
void GameView::setCursorSize( int size )
{
    if( size != cursorSize )
    {
        cursorSize = size;
        setDirty();
    }
}

/*
 * evaluate main_screen_originx and main_screen_originy
 */
void GameView::readOrigin( bool redraw /* = true */ ){
    MapPoint newCenter( main_screen_originx, main_screen_originy );
    show( newCenter, redraw );
}

/*
 * set main_screen_originx and main_screen_originy
 */
void GameView::writeOrigin(){
    main_screen_originx = getCenter().x;
    main_screen_originy = getCenter().y;
}
/*
 *  inform GameView about change in Mini Map Mode
 */
void GameView::setMapMode( MiniMap::DisplayMode mMode ) {
    switch( mMode ){
        case MiniMap::NORMAL:
            printStatusMessage( _("Minimap: outline map") );
            break;
        case MiniMap::UB40:
            printStatusMessage( _("Minimap: unemployment") );
            break;
        case MiniMap::POLLUTION:
            printStatusMessage( _("Minimap: pollution") );
            break;
        case MiniMap::STARVE:
            printStatusMessage( _("Minimap: nourishments") );
            break;
        case MiniMap::POWER:
            printStatusMessage( _("Minimap: power supply") );
            break;
        case MiniMap::FIRE:
            printStatusMessage( _("Minimap: firedepartment cover") );
            break;
        case MiniMap::CRICKET:
            printStatusMessage( _("Minimap: sport cover") );
            break;
        case MiniMap::HEALTH:
            printStatusMessage( _("Minimap: medical care") );
            break;
        case MiniMap::COAL:
            printStatusMessage( _("Minimap: coal deposits") );
            break;
        case MiniMap::TRAFFIC:
        {
            std::string s1 = _("Minimap: traffic density:");
            std::string s2 = commodityNames[getMiniMap()->getStuffID()];
            printStatusMessage( s1 + " " + s2 );
        }
            break;
        case MiniMap::COMMODITIES:
        {
            std::string s1 = _("Minimap: commodities:");
            std::string s2 = commodityNames[getMiniMap()->getStuffID()];
            printStatusMessage( s1 + " " + s2 );
        }
            break;
        default:
            std::cerr << "Unknown minimap mode " << mMode<<"\n";
    }
    if( mapMode == mMode ){
        return;
    }
    mapMode = mMode;
    if( mapOverlay != overlayNone ){
        requestRedraw();
    }
}

/*
 *  Get Tile in Center of Screen.
 */
MapPoint GameView::getCenter(){
    Vector2 center( getWidth() / 2, getHeight() / 2 );
    return getTile( center );
}

/*
 * Adjust the Zoomlevel.
 */
void GameView::setZoom(float newzoom){
    MapPoint centerTile  = getCenter();

    //if ( newzoom < .0625 ) return;
    if ( newzoom < .0312 ) return;
    if ( newzoom > 4 ) return;

    zoom = newzoom;

    // fix rounding errors...
    if(fabs(zoom - 1.0) < .01)
    {   zoom = 1;}

    tileWidth = defaultTileWidth * zoom;
    tileHeight = defaultTileHeight * zoom;
    //a virtual screen containing the whole city
    virtualScreenWidth = tileWidth * world.len();
    virtualScreenHeight = tileHeight * world.len();
    //std::cout << "Zoom " << zoom  << "\n";

    //Show the Center
    show( centerTile );
}

void GameView::zoomMouse(float factor, Vector2 mousepos) {
    float newzoom = zoom * factor;

    //if ( newzoom < .0625 ) return;
    if (newzoom < .0312) {
        newzoom = .0312;
        factor = newzoom / zoom;
    }
    if(newzoom > 4) {
        newzoom = 4;
        factor = newzoom / zoom;
    }

    zoom = newzoom;

    // fix rounding errors...
    if(fabs(zoom - 1.0) < .01)
        zoom = 1;

    tileWidth = defaultTileWidth * zoom;
    tileHeight = defaultTileHeight * zoom;
    //a virtual screen containing the whole city
    virtualScreenWidth = tileWidth * world.len();
    virtualScreenHeight = tileHeight * world.len();
    //std::cout << "Zoom " << zoom  << "\n";

    viewport = (viewport + mousepos) * factor - mousepos;
    constrainViewportPosition(true);

    requestRedraw();
}

/* set Zoomlevel to 100% */
void GameView::resetZoom(){
    setZoom( defaultZoom );
}

/* increase Zoomlevel */
void GameView::zoomIn(){
    setZoom( zoom * sqrt( 2.f ) );
}

/** decrease Zoomlevel */
void GameView::zoomOut(){
    setZoom( zoom / sqrt( 2.f ) );
}

/**
 *  Show City Tile(x/y) by centering the screen
 *  redraw = false is used on initialisation.
 */
void GameView::show( MapPoint map , bool redraw /* = true */ )
{
    Vector2 center;
    center.x = virtualScreenWidth / 2 + ( map.x - map.y ) * ( tileWidth / 2 );
    center.y = ( map.x + map.y ) * ( tileHeight / 2 ) + ( tileHeight / 2 );
    if( redraw ){
        viewport.x = center.x - ( getWidth() / 2 );
        viewport.y = center.y - ( getHeight() / 2 );
        requestRedraw();
    } else { //on startup getWidth is 0.
        viewport.x = center.x - ( getConfig()->videoX / 2 );
        viewport.y = center.y - ( getConfig()->videoY / 2 );
    }
}

/*
 * Loads Texture from filename, Returns Pointer to Texture
 * or Null if no file found. NOT THREADSAFE
 */

Texture* GameView::readTexture(const std::string& filename)
{
    const auto dirsep = "/";
    std::string nfilename = std::string("images") + dirsep
    + std::string("tiles") + dirsep + filename;
    Texture* currentTexture;
    try {
        currentTexture = texture_manager->load(nfilename);
    } catch(std::exception& err) {
        std::cerr << nfilename << "GameView::readTexture# missing: " << err.what() << "\n";
        return 0;
    }
    return currentTexture;
}

/*
 * Loads Image from filename, Returns Pointer to Image
 * or Null if no file found. IS THREADSAFE
 */
SDL_Surface* GameView::readImage(const std::string& filename)
{
    const auto dirsep = "/";

    //std::string nfilename;
    //nfilename = std::string("images") + dirsep + std::string("tiles") + dirsep + filename;

    std::ostringstream os;
    os << "images" << dirsep << "tiles" << dirsep << filename;
    std::string nfilename = os.str();

    SDL_Surface* currentImage;
    if( !PHYSFS_exists( nfilename.c_str() ) ){
        std::cerr << "GameView::readImage# No image file "<< nfilename << " found.\n";
        return 0;
    }
    currentImage = IMG_Load_RW(getPhysfsSDLRWops( nfilename ), 1);
    if( !currentImage ) {
        std::cerr << "GameView::readImage# Could not load image "<< nfilename << "\n";
    }
    return currentImage;
}

/**
 * preload all images and with X and Y offsets.
 * from data/images/tiles/images.xml
 *
 *images and offsets are appended to ConstructionGroup::graphicVectorInfo see lintypes.h
 *the resourceID strings are hard coded in all_modules.cpp and match the names of the sounds
 *
 * some images contain the x-Coordinate of the
 * middle of the Building in Case the Image is asymetric,
 * eg. a high tower with a long shadow to the right
 *
 *  Some of the Image to Texture Conversion seems not to be threadsave
 *  in OpenGL, so load just Images and convert them to Textures on
 *  demand in the main Tread.
 */

void GameView::preReadImages(void)
{
    const auto dirsep = "/";

    std::ostringstream os;
    os << "images" << dirsep << "tiles" << dirsep << "images.xml";
    std::string xmlfile = os.str();

    XmlReader reader( xmlfile );

    ResourceGroup *resourceGroup = 0;
    int resourceID_level = 0;
    std::string key;

    while( reader.read() )
    {
        if( reader.getNodeType() == XML_READER_TYPE_ELEMENT)
        {
            const std::string& element = (const char*) reader.getName();

            if( element == "resourceID")
            {
                XmlReader::AttributeIterator iter(reader);
                while(iter.next())
                {
                    const char* name = (const char*) iter.getName();
                    const char* value = (const char*) iter.getValue();
                    if( strcmp(name, "name" ) == 0 )
                    {
                        if(resourceGroup)
                        {   resourceGroup->images_loaded = true;}

                        if(ResourceGroup::resMap.count(value))
                        {
                            resourceGroup = ResourceGroup::resMap[value];
                            resourceID_level = reader.getDepth();
                            if(resourceGroup->images_loaded)
                            {
                                std::cout << "Duplicate resourceID in images.xml: " << value << std::endl;
                                assert(false);
                            }
                        }
                        else
                        {
                            std::cout << "unknown resourceID: " << value << " in images.xml" << std::endl;
                            resourceGroup = 0;
                            resourceID_level = 0;
                        }
                    }
                }
            }
            //check if we are still inside context of last resorceID
            if(reader.getDepth() < resourceID_level-1)
            {
                resourceGroup->images_loaded = true;
                resourceGroup = 0;
                resourceID_level = 0;
            }
            if( element == "image" )
            {
                XmlReader::AttributeIterator iter(reader);
                int xmlX = 64;
                int xmlY = 32;
                bool xmlX_set = false;
                bool xmlY_set = false;
                while(iter.next())
                {
                    const char* name = (const char*) iter.getName();
                    const char* value = (const char*) iter.getValue();
                    if( strcmp(name, "file" ) == 0 )
                    {   key = value;}
                    else if( strcmp(name, "x" ) == 0 )
                    {
                        if(sscanf(value, "%i", &xmlX) != 1)
                        {
                            std::cerr << "GameView::preReadCityXY# Error parsing integer value '" << value << "' in x attribute.\n";
                        }
                        else
                        {   xmlX_set = true;}
                    }
                   else if(strcmp(name, "y") == 0 )
                    {
                        if(sscanf(value, "%i", &xmlY) != 1)
                        {
                            std::cerr << "GameView::preReadCityXY# Error parsing integer value '" << value << "' in y attribute.\n";
                        }
                        else
                        {   xmlY_set = true;}
                    }
                }

                if (resourceID_level && resourceGroup)
                {
                    resourceGroup->growGraphicsInfoVector();
                    GraphicsInfo *graphicsInfo = &(resourceGroup->graphicsInfoVector.back());
                    graphicsInfo->image = readImage( key );
                    if(!graphicsInfo->image)
                    {
                        std::cout << "image error: " << key << std::endl;
                    }
                    if(!xmlX_set)
                    {   xmlX = int(graphicsInfo->image->w/2);}
                    if(!xmlY_set)
                    {   xmlY = int(graphicsInfo->image->h);}
                    graphicsInfo->x = xmlX;
                    graphicsInfo->y = xmlY;
                    ++remaining_images;
                }
                key.clear();
            }
        }
    }
    if(resourceGroup)
    {
        resourceGroup->images_loaded = true;
        resourceGroup = 0;
    }
}


/*
 * Scroll the map.
 */
void GameView::scroll(float elapsedTime)
{
    int scrollState = keyScrollState | mouseScrollState;
    if( scrollState == SCROLL_NONE ) {
        return;
    }

    //TODO: scroll speed should be configurable
    // The sqrt(zoom) makes it feel like the same speed at different zoom
    // levels.
    float amt = (elapsedTime * 1000) * 0.5 * sqrt(zoom);
    Vector2 dir = Vector2(0,0);

    if( keyScrollState & SCROLL_SHIFT_ALL ) {
        amt *= 4;
    }

    if( scrollState & SCROLL_UP_ALL ) {
        dir.y -= 1;
    }
    if( scrollState & SCROLL_DOWN_ALL ) {
        dir.y += 1;
    }
    if( scrollState & SCROLL_LEFT_ALL ) {
        dir.x -= 1;
    }
    if( scrollState & SCROLL_RIGHT_ALL ) {
        dir.x += 1;
    }

    if(dir == Vector2(0,0)) return;

    // The sqrt((float)tileWidth / tileHeight) makes vertical/horizonal
    // scrolling feel like the same speed. Surprisingly, without the square
    // root, it doesn't feel right.
    float norm = hypot(dir.x * sqrt((float)tileWidth / tileHeight), dir.y);
    // This makes diagonal scrolling parallel to map components.
    dir.x *= (float)tileWidth / tileHeight;
    viewport += dir * amt / norm;
    constrainViewportPosition(false);

    requestRedraw();
}

bool GameView::constrainViewportPosition(bool useScrollCorrection) {
  //If the centre of the Screen is not Part of the city
  //adjust viewport so it is.
  if(useScrollCorrection)
    viewport += scrollCorrection * zoom;
  Vector2 center = viewport + (Vector2(getWidth() - virtualScreenWidth, getHeight()) / 2);
  Vector2 centerTile = Vector2(
    center.y / tileHeight + center.x / tileWidth,
    center.y / tileHeight - center.x / tileWidth
  );
  bool outside = false;
  if(centerTile.x < gameAreaMin) {
      centerTile.x = gameAreaMin;
      outside = true;
  }
  else if(centerTile.x > gameAreaMax() + 1) {
      centerTile.x = gameAreaMax() + 1;
      outside = true;
  }
  if(centerTile.y < gameAreaMin) {
      centerTile.y = gameAreaMin;
      outside = true;
  }
  else if(centerTile.y > gameAreaMax() + 1) {
      centerTile.y = gameAreaMax() + 1;
      outside = true;
  }

  if(outside) {
      Vector2 vpOld = viewport;
      center.x = ( centerTile.x - centerTile.y ) * tileWidth / 2;
      center.y = ( centerTile.x + centerTile.y ) * tileHeight / 2;
      viewport = center - (Vector2(getWidth() - virtualScreenWidth, getHeight()) / 2);
      if(useScrollCorrection)
        scrollCorrection = (vpOld - viewport) / zoom;
      else
        scrollCorrection = Vector2(0,0);
      requestRedraw();
      return true;
  }
  else {
    scrollCorrection = Vector2(0,0);
    return false;
  }
}

void GameView::updateMps(int x, int y) {
    int mod_x = x, mod_y = y;
    if(world(x,y)->reportingConstruction && !mpsEnvOnQuery) {
        mod_x = world(x,y)->reportingConstruction->x;
        mod_y = world(x,y)->reportingConstruction->y;
    }
    mps_set(mod_x, mod_y, mpsEnvOnQuery ? MPS_ENV : MPS_MAP);
}

/*
 * Process event
 */
void GameView::event(const Event& event)
{
    switch(event.type) {
        case Event::MOUSEMOTION: {
            mouseScrollState = SCROLL_NONE;
            if(!dragging) {
                if( event.mousepos.x < scrollBorder ) {
                    mouseScrollState |= SCROLL_LEFT;
                } else if( event.mousepos.x > getWidth() - scrollBorder ) {
                    mouseScrollState |= SCROLL_RIGHT;
                }
                if( event.mousepos.y < scrollBorder ) {
                    mouseScrollState |= SCROLL_UP;
                } else if( event.mousepos.y > getHeight() - scrollBorder ) {
                    mouseScrollState |= SCROLL_DOWN;
                }
            }

            if( dragging ) {
                if(fabsf(event.mousemove.x) < 1 && fabsf(event.mousemove.y) < 1)
                    break;
                // this was most probably a SDL_WarpMouse
                if(event.mousepos == dragStart)
                    break;
                viewport -= event.mousemove;
                constrainViewportPosition(true);
                setDirty();
                break;
            }
            if(!rightButtonDown) {
              // Use `rightButtonDown` instead of `dragging` so releasing and
              // re-pressing the button does not lose the drag correction. Such
              // a release and re-press was probably a mistake.
              scrollCorrection = Vector2(0,0);
            }

            if(!event.inside) {
                mouseInGameView = false;
                break;
            }
            mouseInGameView = true;

            if( !dragging && rightButtonDown ) {
                dragging = true;
                dragStart = event.mousepos;
                dragStartTime = SDL_GetTicks(); // Is this unused???
            }
            MapPoint tile = getTile(event.mousepos);
            if( !roadDragging && leftButtonDown && ( cursorSize == 1 ) &&
            (userOperation->action != UserOperation::ACTION_EVACUATE))
            {
                roadDragging = true;
                startRoad = tile;
                areaBulldoze = (SDL_GetModState() & KMOD_CTRL);
            }

            if( roadDragging && ( cursorSize != 1 ) )
            {
                roadDragging = false;
                areaBulldoze = false;
                ctrDrag = false;
            }
            // bulldoze at once while still dragging

            if( roadDragging && ( (userOperation->action == UserOperation::ACTION_BULLDOZE))
            && !areaBulldoze){
                if( tile != startRoad ) {
                    check_bulldoze_area (startRoad.x, startRoad.y);
                    startRoad = tile;
                }
            }

            if(tileUnderMouse != tile) {
                tileUnderMouse = tile;
                setDirty();
                //update mps target
                if(userOperation->action == UserOperation::ACTION_EVACUATE)
                {   mps_set( tile.x, tile.y, MPS_MAP );}
            }

            break;
        }
        case Event::MOUSEBUTTONDOWN: {
            if(!event.inside) {
                break;
            }
            if( event.mousebutton == SDL_BUTTON_MIDDLE ) {
                dragging = false;
                ctrDrag = false;
                rightButtonDown = true;
                setPanningCursor();
                break;
            }
            if( event.mousebutton == SDL_BUTTON_LEFT ) {
                roadDragging = false;
                areaBulldoze = false;
                ctrDrag = false;
                leftButtonDown = true;
                break;
            }
            if( event.mousebutton == SDL_BUTTON_RIGHT ) {
                if( inCity( getTile( event.mousepos ) ) ) {
                    // getMiniMap()->showMpsEnv( getTile( event.mousepos ) );
                }
            }
            break;
        }
        case Event::MOUSEBUTTONUP:

            if( event.mousebutton == SDL_BUTTON_RIGHT ){
                // getMiniMap()->hideMpsEnv();
            }

            if( event.mousebutton == SDL_BUTTON_MIDDLE ){
                if ( dragging ) {
                    dragging = false;
                    rightButtonDown = false;
                    setDefaultCursor();
                    // getButtonPanel()->selectQueryTool();
                    break;
                }
                dragging = false;
                rightButtonDown = false;
            }
            if( event.mousebutton == SDL_BUTTON_LEFT ){
                resetLastMessage();
                if ( roadDragging && event.inside )
                {
                    MapPoint endRoad = getTile( event.mousepos );
                    roadDragging = false;
                    areaBulldoze = false;
                    leftButtonDown = false;
                    if( cursorSize != 1 ){//roadDragging was aborted with Escape
                        break;
                    }

                    //build last tile first to play the sound
                    if( !blockingDialogIsOpen &&
                        userOperation->is_allowed_here(endRoad.x, endRoad.y, true))
                        {   editMap(endRoad, SDL_BUTTON_LEFT);}
                    //use same method to find all Tiles as in void GameView::draw()
                    MapPoint currentTile = startRoad;
                    int stepx = ( startRoad.x > endRoad.x ) ? -1 : 1;
                    int stepy = ( startRoad.y > endRoad.y ) ? -1 : 1;
                    if ( userOperation->action == UserOperation::ACTION_BULLDOZE )
                    {
                        for (;currentTile.x != endRoad.x + stepx; currentTile.x += stepx)
                        {
                            for (currentTile.y = startRoad.y; currentTile.y != endRoad.y + stepy; currentTile.y += stepy)
                            {
                                if( !blockingDialogIsOpen )
                                {   check_bulldoze_area(currentTile.x, currentTile.y);}
                            }
                        }
                    }
                    else if (userOperation->action == UserOperation::ACTION_BUILD)
                    {
                        int* v1 = ctrDrag ? &currentTile.y :&currentTile.x;
                        int* v2 = ctrDrag ? &currentTile.x :&currentTile.y;
                        int* l1 = ctrDrag ? &endRoad.y :&endRoad.x;
                        int* l2 = ctrDrag ? &endRoad.x :&endRoad.y;
                        int* s1 = ctrDrag ? &stepy: &stepx;
                        int* s2 = ctrDrag ? &stepx: &stepy;
                        ConstructionGroup* cstgrp = userOperation->constructionGroup;
                        unsigned short size = cstgrp->size;

                        while( *v1 != *l1 )
                        {
                            if(userOperation->is_allowed_here(currentTile.x, currentTile.y, false))
                            {
                                cstgrp->placeItem(currentTile.x, currentTile.y);
                                connect_transport(currentTile.x - 2, currentTile.y - 2, currentTile.x + size + 1, currentTile.y + size + 1);
                                desert_water_frontiers(currentTile.x - 1, currentTile.y - 1, size + 2, size + 2);
                            }
                            *v1 += *s1;
                        }

                        while( *v2 != *l2 )
                        {
                            if(userOperation->is_allowed_here(currentTile.x, currentTile.y, false))
                            {
                                userOperation->constructionGroup->placeItem(currentTile.x, currentTile.y);
                                connect_transport(currentTile.x - 2, currentTile.y - 2, currentTile.x + size + 1, currentTile.y + size + 1);
                                desert_water_frontiers(currentTile.x - 1, currentTile.y - 1, size + 2, size + 2);
                            }
                            *v2 += *s2;
                        }
                    }
                    break;
                }
                roadDragging = false;
                ctrDrag = false;
                areaBulldoze = false;
                leftButtonDown = false;
            }
            if(!event.inside) {
                break;
            }

            if( event.mousebutton == SDL_BUTTON_LEFT ){                 //left
                if( !blockingDialogIsOpen ) //edit tile
                {   editMap( getTile( event.mousepos ), SDL_BUTTON_LEFT);}
            }
            else if( event.mousebutton == SDL_BUTTON_RIGHT ){           //middle
                // show info on the clicked thing
                MapPoint point = getTile(event.mousepos);
                if(!inCity(point)) break;
                updateMps(point.x, point.y);
            }
            break;
        case Event::MOUSEWHEEL:
            if (!event.inside || event.scrolly == 0)
                break;
            if (event.scrolly > 0)
                zoomMouse(sqrt(2.f), event.mousepos);
            else
                zoomMouse(sqrt(0.5), event.mousepos);
            break;
        case Event::WINDOWLEAVE:
            mouseInGameView = false;
            mouseScrollState = SCROLL_NONE;
            break;
        case Event::WINDOWENTER:
            break;

        case Event::KEYDOWN:
            if( event.keysym.scancode == SDL_SCANCODE_LCTRL || event.keysym.scancode == SDL_SCANCODE_RCTRL ){
                if (roadDragging)
                {   ctrDrag = !ctrDrag;}
                break;
            }
            if( event.keysym.scancode == SDL_SCANCODE_KP_8 ||
                event.keysym.scancode == SDL_SCANCODE_UP ||
                event.keysym.scancode == SDL_SCANCODE_W
            ){
                keyScrollState |= SCROLL_UP;
                break;
            }
            if( event.keysym.scancode == SDL_SCANCODE_KP_2 ||
                event.keysym.scancode == SDL_SCANCODE_DOWN ||
                event.keysym.scancode == SDL_SCANCODE_S
            ){
                keyScrollState |= SCROLL_DOWN;
                break;
            }
            if( event.keysym.scancode == SDL_SCANCODE_KP_4 ||
                event.keysym.scancode == SDL_SCANCODE_LEFT ||
                event.keysym.scancode == SDL_SCANCODE_A
            ){
                keyScrollState |= SCROLL_LEFT;
                break;
            }
            if( event.keysym.scancode == SDL_SCANCODE_KP_6 ||
                event.keysym.scancode == SDL_SCANCODE_RIGHT ||
                event.keysym.scancode == SDL_SCANCODE_D
            ){
                keyScrollState |= SCROLL_RIGHT;
                break;
            }
            if( event.keysym.scancode == SDL_SCANCODE_KP_7 ){
                keyScrollState |= SCROLL_UP_LEFT;
                break;
            }
            if( event.keysym.scancode == SDL_SCANCODE_KP_9 ){
                keyScrollState |= SCROLL_UP_RIGHT;
                break;
            }
            if( event.keysym.scancode == SDL_SCANCODE_KP_1 ){
                keyScrollState |= SCROLL_DOWN_LEFT;
                break;
            }
            if( event.keysym.scancode == SDL_SCANCODE_KP_3 ){
                keyScrollState |= SCROLL_DOWN_RIGHT;
                break;
            }
            if( event.keysym.scancode == SDL_SCANCODE_LSHIFT ){
                keyScrollState |= SCROLL_LSHIFT;
                break;
            }
            if( event.keysym.scancode == SDL_SCANCODE_RSHIFT ){
                keyScrollState |= SCROLL_RSHIFT;
                break;
            }


            // use G to show ground info aka MpsEnv without middle mouse button
            if( event.keysym.scancode == SDL_SCANCODE_G){
                // if( inCity(tileUnderMouse) ) {
                //     getMiniMap()->showMpsEnv( tileUnderMouse );
                // }
                break;
            }
            // hotkeys for scrolling pages up and down
            if(event.keysym.scancode == SDL_SCANCODE_N)
            {
                getMiniMap()->scrollPageDown(true);
                break;
            }
            if (event.keysym.scancode == SDL_SCANCODE_M)
            {
                getMiniMap()->scrollPageDown(false);
                break;
            }
            break;
        case Event::KEYUP:
/*
            //TEst
            if( event.keysym.scancode == SDL_SCANCODE_X ){
                writeOrigin();
                readOrigin();
                break;
            }
*/

            if( event.keysym.scancode == SDL_SCANCODE_G ){
                mpsEnvOnQuery = !mpsEnvOnQuery;
                updateMps(mps_x, mps_y);
                break;
            }
            //Hide High Buildings
            if( event.keysym.scancode == SDL_SCANCODE_H ){
                if( hideHigh ){
                    hideHigh = false;
                } else {
                    hideHigh = true;
                }
                requestRedraw();
                break;
            }
            //overlay MiniMap Information
            if( event.keysym.scancode == SDL_SCANCODE_V ){
                mapOverlay = (mapOverlay+1)%(overlayMAX+1);
                requestRedraw();
                break;
            }
            //Zoom
            if( event.keysym.sym == SDLK_KP_PLUS ){
                zoomIn();
                break;
            }
            if( event.keysym.sym == SDLK_KP_MINUS ){
                zoomOut();
                break;
            }
            if( event.keysym.scancode == SDL_SCANCODE_RETURN
                    || event.keysym.scancode == SDL_SCANCODE_RETURN2) {
                resetZoom();
                break;
            }
            //Scroll
            if( event.keysym.scancode == SDL_SCANCODE_KP_8 ||
                event.keysym.scancode == SDL_SCANCODE_UP ||
                event.keysym.scancode == SDL_SCANCODE_W
            ){
                keyScrollState &= ~SCROLL_UP;
                break;
            }
            if( event.keysym.scancode == SDL_SCANCODE_KP_2 ||
                event.keysym.scancode == SDL_SCANCODE_DOWN ||
                event.keysym.scancode == SDL_SCANCODE_S
            ){
                keyScrollState &= ~SCROLL_DOWN;
                break;
            }
            if( event.keysym.scancode == SDL_SCANCODE_KP_4 ||
                event.keysym.scancode == SDL_SCANCODE_LEFT ||
                event.keysym.scancode == SDL_SCANCODE_A
            ){
                keyScrollState &= ~SCROLL_LEFT;
                break;
            }
            if( event.keysym.scancode == SDL_SCANCODE_KP_6 ||
                event.keysym.scancode == SDL_SCANCODE_RIGHT ||
                event.keysym.scancode == SDL_SCANCODE_D
            ){
                keyScrollState &= ~SCROLL_RIGHT;
                break;
            }
            if( event.keysym.scancode == SDL_SCANCODE_KP_7 ){
                keyScrollState &= ~SCROLL_UP_LEFT;
                break;
            }
            if( event.keysym.scancode == SDL_SCANCODE_KP_9 ){
                keyScrollState &= ~SCROLL_UP_RIGHT;
                break;
            }
            if( event.keysym.scancode == SDL_SCANCODE_KP_1 ){
                keyScrollState &= ~SCROLL_DOWN_LEFT;
                break;
            }
            if( event.keysym.scancode == SDL_SCANCODE_KP_3 ){
                keyScrollState &= ~SCROLL_DOWN_RIGHT;
                break;
            }
            if( event.keysym.scancode == SDL_SCANCODE_LSHIFT ){
                keyScrollState &= ~SCROLL_LSHIFT;
                break;
            }
            if( event.keysym.scancode == SDL_SCANCODE_RSHIFT ){
                keyScrollState &= ~SCROLL_RSHIFT;
                break;
            }

            if ( event.keysym.scancode == SDL_SCANCODE_KP_5 ) {
                show(MapPoint(world.len() / 2, world.len() / 2));
                setDirty();
                break;
            }
            break;

        case Event::UPDATE:
            scroll(event.elapsedTime);
            break;

        default:
            break;
    }
}

void GameView::setPanningCursor() {
    desktop->setSystemCursor(this, SDL_SYSTEM_CURSOR_SIZEALL);
}

void GameView::setDefaultCursor() {
    desktop->tryClearCursor(this);
}

/*
 * Parent tells us to change size.
 */
void GameView::resize(float newwidth , float newheight )
{
    width = newwidth;
    height = newheight;
    if(width < 0) width = 0;
    if(height < 0) height = 0;
    requestRedraw();
}

/*
 *  We should draw the whole City again.
 */
void GameView::requestRedraw()
{
    if( !getMiniMap() ){ //initialization not completed
        return;
    }

    static MapPoint oldCenter = this->getCenter();
    static float oldZoom = zoom;

    MapPoint newCenter = this->getCenter();

    if ( (oldCenter != newCenter) || (oldZoom != zoom) ) {
        //Tell Minimap about new Corners
        getMiniMap()->setGameViewCorners(
            getTile(Vector2(0, 0)),
            getTile(Vector2(getWidth(), getHeight()))
        );

        oldCenter = newCenter;
        oldZoom = zoom;

    }

    //request redraw
    setDirty();
}

/*
 * Pos is new Center of the Screen
 */
void GameView::recenter(const Vector2& pos)
{
    Vector2 position = pos + viewport;
    viewport.x = floor( position.x - ( getWidth() / 2 ) );
    viewport.y = floor( position.y - ( getHeight() / 2 ) );

    //request redraw
    requestRedraw();
}

/*
 * Find point on Screen, where lower right corner of tile
 * is placed.
 */
Vector2 GameView::getScreenPoint(MapPoint map)
{
    Vector2 point;
    point.x = virtualScreenWidth / 2 + (map.x - map.y) * ( tileWidth / 2 );
    point.y = (map.x + map.y) * ( tileHeight / 2 );

    //we want the lower right corner
    point.y += tileHeight;

    if ((showTerrainHeight) && (inCity(map))){
        // shift the tile upward to show altitude
        point.y -= (float) ( (world(map.x, map.y)->ground.altitude) * scale3d) * zoom  / (float) alt_step ;
    }

    //on Screen
    point -= viewport;

    return point;
}

/*
 * Find Tile at point on viewport
 */
MapPoint GameView::getTile(const Vector2& p)
{
    MapPoint tile;
    // Map Point to virtual Screen
    Vector2 point = p + viewport;
    float x = (point.x - virtualScreenWidth / 2 ) / tileWidth
        +  point.y  / tileHeight;
    tile.x = (int) floorf(x);
    tile.y = (int) floorf( 2 * point.y  / tileHeight  - x );

    return tile;
}

/*
 * Draw a filled Diamond inside given Rectangle
 */
void GameView::fillDiamond( Painter& painter, const Rect2D& rect )
{
    Vector2 points[ 4 ];
    points[ 0 ].x = rect.p1.x + ( rect.getWidth() / 2 );
    points[ 0 ].y = rect.p1.y;
    points[ 1 ].x = rect.p1.x;
    points[ 1 ].y = rect.p1.y + ( rect.getHeight() / 2 );
    points[ 2 ].x = rect.p1.x + ( rect.getWidth() / 2 );
    points[ 2 ].y = rect.p2.y;
    points[ 3 ].x = rect.p2.x;
    points[ 3 ].y = rect.p1.y + ( rect.getHeight() / 2 );
    painter.fillPolygon( 4, points );
}

/*
 * Draw a outlined Diamond inside given Rectangle
 */
void GameView::drawDiamond( Painter& painter, const Rect2D& rect )
{
    if( !buttonsConnected ) connectButtons();//has to be done here,
                    // because now everything else is initialized.
    Vector2 points[ 4 ];
    points[ 0 ].x = rect.p1.x + ( rect.getWidth() / 2 );
    points[ 0 ].y = rect.p1.y;
    points[ 1 ].x = rect.p1.x;
    points[ 1 ].y = rect.p1.y + ( rect.getHeight() / 2 );
    points[ 2 ].x = rect.p1.x + ( rect.getWidth() / 2 );
    points[ 2 ].y = rect.p2.y;
    points[ 3 ].x = rect.p2.x;
    points[ 3 ].y = rect.p1.y + ( rect.getHeight() / 2 );
    painter.drawPolygon( 4, points );
}

/*
 * Check if Tile is in City
 * in oldgui you can edit (1,1) to (98,98) with world.len() 100
 * i.e. there is a hidden border of green tiles arround the city
 */
bool GameView::inCity( MapPoint tile )
{
    return world.is_visible(tile.x, tile.y);
}

/*
 * Draw MiniMapOverlay for tile.
 */
void GameView::drawOverlay(Painter& painter, const MapPoint &tile){
    Color black;
    black.parse("black");
    Color miniMapColor;

    Vector2 tileOnScreenPoint = getScreenPoint(tile);
    Rect2D tilerect( 0, 0, tileWidth, tileHeight );
    tileOnScreenPoint.x = tileOnScreenPoint.x - ( tileWidth / 2);
    tileOnScreenPoint.y -= tileHeight;
    tilerect.move( tileOnScreenPoint );
    //Outside of the Map gets Black overlay
    if( !inCity( tile ) ) {
            painter.setFillColor( black );
    } else {
        miniMapColor = getMiniMap()->getColor( tile.x, tile.y );
        if( mapOverlay == overlayOn ){
            miniMapColor.a = 200;  //Transparent
        }
        painter.setFillColor( miniMapColor );
    }
    fillDiamond( painter, tilerect );
}

/*
 * If the current Tile is Part of a Building, return the
 * Coordinates of the tile that contains the real informations.
 */
MapPoint GameView::realTile( MapPoint tile )
{
    MapPoint real = tile;
    if( ! inCity( tile ) )
        return real;
    if(world(tile.x, tile.y)->reportingConstruction)
    {
        real.x = world(tile.x, tile.y)->reportingConstruction->x;
        real.y = world(tile.x, tile.y)->reportingConstruction->y;
        return real;
    }
    return real;
}

void GameView::fetchTextures()
{
    std::map<std::string, ResourceGroup*>::iterator it;
    for(it= ResourceGroup::resMap.begin(); it != ResourceGroup::resMap.end(); ++it)
    {
        for(size_t i = 0; i < it->second->graphicsInfoVector.size(); ++i)
        {
            if( !it->second->graphicsInfoVector[i].texture)
            {
                if(it->second->graphicsInfoVector[i].image)
                {
                    it->second->graphicsInfoVector[i].texture = texture_manager->create( it->second->graphicsInfoVector[i].image );
                    if (it->second->graphicsInfoVector[i].texture)
                    {   it->second->graphicsInfoVector[i].image = 0;} //Image was erased by texture_manager->create.
                    --remaining_images;
                }
            }
        }
    }
}



void GameView::drawTexture(Painter& painter, const MapPoint &tile, GraphicsInfo *graphicsInfo)
{
    Rect2D tilerect( 0, 0, tileWidth, tileHeight );
    Vector2 tileOnScreenPoint = getScreenPoint( tile );
    // Test if we have to convert Preloaded Image to Texture
    if( !graphicsInfo->texture ) //&& !economyGraph_open
    {
        if(graphicsInfo->image)
        {
            graphicsInfo->texture = texture_manager->create( graphicsInfo->image );
            if ( graphicsInfo->texture)
            {   graphicsInfo->image = 0;} //Image was erased by texture_manager->create.
            --remaining_images;
        }
    }
    if (graphicsInfo->texture)
    {
        tileOnScreenPoint.x -= graphicsInfo->x * zoom;
        tileOnScreenPoint.y -= graphicsInfo->y * zoom;
        tilerect.move( tileOnScreenPoint );
        tilerect.setSize(graphicsInfo->texture->getWidth() * zoom,
            graphicsInfo->texture->getHeight() * zoom);
        if( zoom == 1.0 )     // Floating point test of equality !
        {    painter.drawTexture(graphicsInfo->texture, tilerect.p1);}
        else
        {   painter.drawStretchTexture(graphicsInfo->texture, tilerect);}
    }
}


void GameView::drawTile(Painter& painter, const MapPoint &tile)
{

    //is Tile in City? If not draw Blank
    if( ! inCity( tile ) )
    {
        drawTexture(painter, tile, &blankGraphicsInfo);
        return;
    }

    //Texture* texture = 0;
    MapPoint upperLeft = realTile(tile);
    int x = upperLeft.x;
    int y = upperLeft.y;

    ConstructionGroup *cstgrp = world(x,y)->getTopConstructionGroup();
    ResourceGroup *resgrp;
    unsigned short size = cstgrp->size;

    //Attention map is rotated for displaying
    if ( ( tile.x == x ) && ( tile.y - size +1 == y ) ) //Signs are tested
    {
        resgrp = world(x, y)->getTileResourceGroup();
        //adjust OnScreenPoint of big Tiles
        MapPoint lowerRightTile( tile.x + size - 1 , tile.y );
        unsigned short textureType = world(x, y)->getTopType();

        // if we hide high buildings, hide trees as well
        if (hideHigh && (cstgrp == &treeConstructionGroup
         || cstgrp == &tree2ConstructionGroup
         || cstgrp == &tree3ConstructionGroup ))
        {
            cstgrp = &bareConstructionGroup;
            resgrp = ResourceGroup::resMap["Green"];
        }
        GraphicsInfo *graphicsInfo = 0;
        //draw visible tiles underneath constructions
        if( (world(x, y)->reportingConstruction || world(x,y)->framesptr) && !(world(x,y)->flags & FLAG_INVISIBLE) )
        {
            if (resgrp->images_loaded)
            {
                size_t s = resgrp->graphicsInfoVector.size();
                if (s)
                {
                    graphicsInfo = &resgrp->graphicsInfoVector
                        [ world(x, y)->type  % s];
                    drawTexture(painter, lowerRightTile, graphicsInfo);
                }
            }
        }
        bool draw_colored_site = true;

        if( (size==1 || !hideHigh) )
        {
            draw_colored_site = false;
            if (world(x,y)->framesptr)
            {
                for(std::list<ExtraFrame>::iterator frit = world(x, y)->framesptr->begin();
                    frit != world(x,y)->framesptr->end(); std::advance(frit, 1))
                {
                    if(frit->resourceGroup && frit->resourceGroup->images_loaded)
                    {
                        size_t s2 = frit->resourceGroup->graphicsInfoVector.size();
                        if((frit->frame >= 0) && s2)
                        {
                            graphicsInfo = &frit->resourceGroup->graphicsInfoVector[ frit->frame % s2 ]; //needed
                            int old_x = graphicsInfo->x;
                            int old_y = graphicsInfo->y;
                            graphicsInfo->x = old_x - frit->move_x;
                            graphicsInfo->y = old_y - frit->move_y;
                            if( frit->resourceGroup->is_vehicle)
                            {
                                //only draw vehicles on top of...
                                switch (cstgrp->group)
                                {
                                    case GROUP_BARE:
                                    case GROUP_DESERT:
                                    case GROUP_WATER:
                                    case GROUP_TRACK:
                                    case GROUP_TRACK_BRIDGE:
                                    case GROUP_ROAD:
                                    case GROUP_ROAD_BRIDGE:
                                    case GROUP_RAIL:
                                    case GROUP_RAIL_BRIDGE:
                                    case GROUP_ORGANIC_FARM:
                                    case GROUP_WATERWELL:
                                    case GROUP_SHANTY:
                                    case GROUP_CRICKET:
                                    case GROUP_SUBSTATION:
                                    //add anything else with low northen corner
                                        drawTexture(painter, upperLeft, graphicsInfo);
                                    break;
                                    default:
                                    break;
                                }


                            }
                            else
                            {   drawTexture(painter, lowerRightTile, graphicsInfo);}
                            graphicsInfo->x = old_x;
                            graphicsInfo->y = old_y;
                        }
                    }
                    else
                    {   draw_colored_site = true;}
                }
            }
            else
            {
                size_t s = resgrp->graphicsInfoVector.size();
                if(s && resgrp->images_loaded)
                {
                    graphicsInfo = &resgrp->graphicsInfoVector[ textureType %  s]; //needed
                    drawTexture(painter, lowerRightTile, graphicsInfo);
                }
            }
        }

        if(draw_colored_site)
        {
            Rect2D tilerect( 0, 0, size * tileWidth, size * tileHeight );
            Vector2 tileOnScreenPoint = getScreenPoint( lowerRightTile );
            tileOnScreenPoint.x =  tileOnScreenPoint.x - ( tileWidth*size / 2);
            tileOnScreenPoint.y -= tileHeight*size;
            tilerect.move( tileOnScreenPoint );
            painter.setFillColor( getMiniMap()->getColorNormal( tile.x, tile.y ) );
            fillDiamond( painter, tilerect );
        }
        //last draw suspended power cables on top
        //only works for size == 1
        if (world(x, y)->flags & (FLAG_POWER_CABLES_0 | FLAG_POWER_CABLES_90))
        {
            resgrp = ResourceGroup::resMap["PowerLine"];
            if(resgrp->images_loaded)
            {
                if (world(x, y)->flags & FLAG_POWER_CABLES_0)
                {   drawTexture(painter, upperLeft, &resgrp->graphicsInfoVector[23]);}
                if (world(x, y)->flags & FLAG_POWER_CABLES_90)
                {   drawTexture(painter, upperLeft, &resgrp->graphicsInfoVector[22]);}
            }
        }
    }


}

/*
 * Mark a tile with current cursor
 */
void GameView::markTile( Painter& painter, const MapPoint &tile )
{
    Vector2 tileOnScreenPoint = getScreenPoint(tile);
    int x = tile.x;
    int y = tile.y;
    {
        MapPoint upperLeft = realTile(tile);
        if(upperLeft.x == mps_x && upperLeft.y == mps_y && userOperation->action == UserOperation::ACTION_QUERY)
        {
            if(world(x,y)->reportingConstruction)
            {
                ConstructionGroup *constructionGroup = world(x,y)->reportingConstruction->constructionGroup;
                int range = constructionGroup->range;
                int edgelen = 2 * range + constructionGroup->size ;
                painter.setFillColor( Color( 0, 255, 0, 64 ) );
                Rect2D rangerect( 0,0,
                                  tileWidth  * ( edgelen) ,
                                  tileHeight * ( edgelen) );
                Vector2 screenPoint = getScreenPoint(upperLeft);
                screenPoint.x -= tileWidth  * ( 0.5*(edgelen) );
                screenPoint.y -= tileHeight * ( range + 1 );
                rangerect.move( screenPoint );
                fillDiamond( painter, rangerect );
            }//endif
        }//endif mps
    }

    if( userOperation->action == UserOperation::ACTION_QUERY) //  cursorSize == 0
    {
        Color alphawhite( 255, 255, 255, 128 );
        painter.setLineColor( alphawhite );
        Rect2D tilerect( 0, 0, tileWidth, tileHeight );
        tileOnScreenPoint.x -= ( tileWidth / 2);
        tileOnScreenPoint.y -= tileHeight;
        tilerect.move( tileOnScreenPoint );
        drawDiamond( painter, tilerect );
    }
    else
    {
        Color alphablue( 0, 0, 255, 128 );
        Color alphared( 255, 0, 0, 128 );
        if(userOperation->is_allowed_here(x, y, false))
        {   painter.setFillColor( alphablue );}
        else
        {   painter.setFillColor( alphared );}

        Rect2D tilerect( 0, 0, tileWidth * cursorSize, tileHeight * cursorSize );
        tileOnScreenPoint.x -= (tileWidth * cursorSize / 2);
        tileOnScreenPoint.y -= tileHeight;
        tilerect.move( tileOnScreenPoint );
        fillDiamond( painter, tilerect );

        if(userOperation->action == UserOperation::ACTION_BUILD)
        {
            // Draw range for selected_building
            int range = userOperation->constructionGroup->range;
            if (range > 0 )
            {
                int edgelen = 2 * range + userOperation->constructionGroup->size ;
                painter.setFillColor( Color( 0, 0, 128, 64 ) );
                Rect2D rangerect( 0,0,
                                  tileWidth  * ( edgelen) ,
                                  tileHeight * ( edgelen) );
                Vector2 screenPoint = getScreenPoint(tile);
                screenPoint.x -= tileWidth  * ( 0.5*edgelen );
                screenPoint.y -= tileHeight * ( range + 1 );
                rangerect.move( screenPoint );
                fillDiamond( painter, rangerect );
            }//endif range > 0
        }
    }//endelse ACTION_QUERY
}

/*
 *  Paint an isometric View of the City in the component.
 */
void GameView::draw(Painter& painter)
{
    // Constraining the position here shouldn't be necessary if other parts of the code are correct.
    // if( constrainViewportPosition(true) ) {
    //     // Returning causes the display to lag. I'm not sure why it's needed anyway.
    //     // return;
    // }

    //The Corners of The Screen
    Vector2 upperLeft( 0, 0);
    Vector2 upperRight( getWidth(), 0 );
    Vector2 lowerLeft( 0, getHeight() );

    if (showTerrainHeight)
    {
        // printf("h = %f,     z = %f \n ", getHeight(), zoom);
        // getHeight = size in pixel of the screen (eg 1024x768)
        Vector2 lowerLeft( 0, getHeight() * ( 1 + getHeight() * zoom / (float)scale3d ));
    }

    //Find visible Tiles
    MapPoint upperLeftTile  = getTile( upperLeft );
    MapPoint upperRightTile = getTile( upperRight );
    MapPoint lowerLeftTile  = getTile( lowerLeft );

    //draw Background
    Color black;
    Rect2D background( 0, 0, getWidth(), getHeight() );
    black.parse( "black" );
    painter.setFillColor( black );
    painter.fillRectangle( background );

    //draw Tiles
    MapPoint currentTile;
    //Draw some extra tiles depending on the maximal size of a building.
    int extratiles = 7;
    upperLeftTile.x -= extratiles;
    upperRightTile.y -= extratiles;
    upperRightTile.x += extratiles;
    lowerLeftTile.y +=  extratiles;

    if (mapOverlay != overlayOnly)
    {
        for(int k = 0; k <= 2 * ( lowerLeftTile.y - upperLeftTile.y ); k++ )
        {
            for(int i = 0; i <= upperRightTile.x - upperLeftTile.x; i++ )
            {
                currentTile.x = upperLeftTile.x + i + k / 2 + k % 2;
                currentTile.y = upperLeftTile.y - i + k / 2;
                drawTile( painter, currentTile );
            }
        }
    }
    if( mapOverlay != overlayNone )
    {
        for(int k = 0; k <= 2 * ( lowerLeftTile.y - upperLeftTile.y ); k++ )
        {
            for(int i = 0; i <= upperRightTile.x - upperLeftTile.x; i++ )
            {
                currentTile.x = upperLeftTile.x + i + k / 2 + k % 2;
                currentTile.y = upperLeftTile.y - i + k / 2;
                drawOverlay( painter, currentTile );
            }
        }
    }

    int cost = 0;
    //display commodities continously
    if(userOperation->action == UserOperation::ACTION_EVACUATE)
    {
        mps_update();
        mps_refresh();
    }
    //Mark Tile under Mouse // TODO: handle showTerrainHeight
    if( mouseInGameView  && !blockingDialogIsOpen ) {
        MapPoint lastRazed( -1,-1 );
        int tiles = 0;
        if( roadDragging && ( cursorSize == 1 ) &&
        (userOperation->action == UserOperation::ACTION_BUILD || userOperation->action == UserOperation::ACTION_BULLDOZE))
        {
            //use same method to find all Tiles as in GameView::event(const Event& event)
            int stepx = ( startRoad.x > tileUnderMouse.x ) ? -1 : 1;
            int stepy = ( startRoad.y > tileUnderMouse.y ) ? -1 : 1;
            currentTile = startRoad;

            if (userOperation->action == UserOperation::ACTION_BULLDOZE)
            {
                for (;currentTile.x != tileUnderMouse.x + stepx; currentTile.x += stepx) {
                    for (currentTile.y = startRoad.y; currentTile.y != tileUnderMouse.y + stepy; currentTile.y += stepy) {
                        markTile( painter, currentTile );
                        if( realTile( currentTile ) != lastRazed ){
                            cost += bulldozeCost( currentTile );
                            lastRazed = realTile( currentTile );
                        }
                        tiles++;
                    }
                }
            }
            else if (userOperation->action == UserOperation::ACTION_BUILD)
            {
                int* v1 = ctrDrag ? &currentTile.y :&currentTile.x;
                int* v2 = ctrDrag ? &currentTile.x :&currentTile.y;
                int* l1 = ctrDrag ? &tileUnderMouse.y :&tileUnderMouse.x;
                int* l2 = ctrDrag ? &tileUnderMouse.x :&tileUnderMouse.y;
                int* s1 = ctrDrag ? &stepy: &stepx;
                int* s2 = ctrDrag ? &stepx: &stepy;

                while( *v1 != *l1)
                {
                    markTile( painter, currentTile );
                    cost += buildCost( currentTile );
                    tiles++;
                    *v1 += *s1;
                }
                while( *v2 != *l2 + *s2 )
                {
                    markTile( painter, currentTile );
                    cost += buildCost( currentTile );
                    tiles++;
                    *v2 += *s2;
                }

            }
        }
        else
        {
            markTile( painter, tileUnderMouse );
            tiles++;
            if( (userOperation->action == UserOperation::ACTION_BULLDOZE ) && realTile( currentTile ) != lastRazed ) {
                    cost += bulldozeCost( tileUnderMouse );
            } else {
                cost += buildCost( tileUnderMouse );
            }
        }
        std::stringstream prize;
        if( userOperation->action == UserOperation::ACTION_BULLDOZE ){
            if( roadDragging ){
                prize << _("Estimated Bulldoze Cost: ");
            } else {
                prize << _("Bulldoze Cost: ");
            }
            if( cost > 0 ) {
                prize << cost << _("$");
            } else {
                prize << _("n/a");
            }
            printStatusMessage( prize.str() );
        }
        else if( userOperation->action == UserOperation::ACTION_BUILD)
        {
            std::string buildingName =  userOperation->constructionGroup->name;
            prize << dictionaryManager->get_dictionary().translate( buildingName );
            prize << _(": Cost to build ");
            if( cost > 0 ) {
                prize << cost << _("$");
        } else {
                prize << _("n/a");
            }
            printStatusMessage( prize.str() );
        } else {
           showToolInfo( tiles );
        }
    }
}

/*
 * Show informatiosn about selected Tool
 */
void GameView::showToolInfo( int number /*= 0*/ )
{
    std::stringstream infotextstream;

    if( userOperation->action == UserOperation::ACTION_QUERY ) //query
    {
        infotextstream << _("Query Tool: Show information about selected building.");
    }
    else if( userOperation->action == UserOperation::ACTION_BULLDOZE ) //bulldoze
    {
        infotextstream << _("Bulldozer: remove building -price varies-");
    }
    else if( userOperation->action == UserOperation::ACTION_BUILD )
    {
        infotextstream << userOperation->constructionGroup->getName();
        infotextstream << _(": Cost to build ") << userOperation->constructionGroup->getCosts() <<_("$");
        infotextstream << _(", to bulldoze ") << userOperation->constructionGroup->bul_cost <<_("$") << ".";
        if( number > 1 ){
            infotextstream << _(" To build ") << number << _(" of them ");
            infotextstream << _("will cost about ") << number*userOperation->constructionGroup->getCosts() << _("$") << "-";
        }
    }
    else if ( userOperation->action == UserOperation::ACTION_EVACUATE )
    {
        infotextstream << _("Evacuation of commodities is for free.");
    }
    else if ( userOperation->action == UserOperation::ACTION_FLOOD )
    {
        infotextstream <<  _("Water") << _(": Cost to build ") << GROUP_WATER_COST << _("$");
        infotextstream << _(", to bulldoze ") << GROUP_WATER_BUL_COST << _("$") << ".";
    }

    printStatusMessage( infotextstream.str() );
}

/*
 * Print a Message to the StatusBar.
 */
void GameView::printStatusMessage( std::string message ){
    if( message == lastStatusMessage ){
        return;
    }
    Component* root = this;
    while( root->getParent() )
        root = root->getParent();
    Desktop* desktop = dynamic_cast<Desktop*> (root);
    if(!desktop) {
        std::cerr << "Root not a desktop!?!\n";
        return;
    }

    try {
        Paragraph* statusParagraph = getParagraph( *root, "statusParagraph");
        statusParagraph->setText( message );
    } catch(std::exception& e) {
        std::cerr << "Couldn't print status message '" << message  << "': "
            << e.what() << "\n";
        return;
    }
    lastStatusMessage = message;
}

int GameView::bulldozeCost( MapPoint tile ){

    if (!world.is_visible(tile.x, tile.y))
    {   return 0;}
    Construction *reportingConstruction = world(tile.x, tile.y)->reportingConstruction;
    if (reportingConstruction)
    {   return reportingConstruction->constructionGroup->bul_cost;}
    else
    {
        int group = world(tile.x, tile.y)->getGroup();
        if (group == GROUP_DESERT)
        {   return 0;}
        else if (group == GROUP_WATER)
        {   return GROUP_WATER_BUL_COST;}
        else
        {   return 1;}
    }
    return 0;
}

int GameView::buildCost( MapPoint tile )
{
    if( !userOperation->is_allowed_here(tile.x, tile.y, false) ||
        userOperation->action == UserOperation::ACTION_QUERY ||
        userOperation->action == UserOperation::ACTION_EVACUATE)
    {   return 0;}
    if( userOperation->action == UserOperation::ACTION_BUILD)
    {   if (world(tile.x, tile.y)->is_water()) //building a bridge
        {   return BRIDGE_FACTOR * userOperation->constructionGroup->getCosts();}
        else //building on land
        {   return userOperation->constructionGroup->getCosts();} userOperation->constructionGroup->getCosts();}
    if (userOperation->action == UserOperation::ACTION_FLOOD &&
        userOperation->is_allowed_here(tile.x, tile.y, false))
    {
        return GROUP_WATER_COST;
    }
    return 0;
}

//Register as Component
IMPLEMENT_COMPONENT_FACTORY(GameView)

/** @file lincity-ng/GameView.cpp */
