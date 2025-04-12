/* ---------------------------------------------------------------------- *
 * src/lincity-ng/GameView.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2005      Wolfgang Becker <uafr@gmx.de>
 * Copyright (C) 2025      David Bears <dbear4q@gmail.com>
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

#include "GameView.hpp"

#include <SDL.h>                         // for SDL_Scancode, SDL_BUTTON_LEFT
#include <SDL_image.h>                   // for IMG_Load
#include <assert.h>                      // for assert
#include <stdio.h>                       // for size_t, sscanf, NULL
#include <string.h>                      // for strcmp
#include <cmath>                         // for sqrt, fabs, fabsf, floorf
#include <exception>                     // for exception
#include <functional>                    // for bind, _1, function
#include <iostream>                      // for basic_ostream, operator<<
#include <iterator>                      // for advance
#include <list>                          // for _List_iterator, list, operat...
#include <map>                           // for _Rb_tree_iterator, map, oper...
#include <sstream>                       // for basic_stringstream
#include <utility>                       // for pair
#include <vector>                        // for vector

#include "Config.hpp"                    // for getConfig, Config
#include "Dialog.hpp"                    // for blockingDialogIsOpen
#include "Game.hpp"                      // for Game
#include "MiniMap.hpp"                   // for MiniMap, getMiniMap
#include "Mps.hpp"                       // for MpsMap
#include "Util.hpp"                      // for getButton, getParagraph
#include "gui/Button.hpp"                // for Button
#include "gui/Color.hpp"                 // for Color
#include "gui/ComponentFactory.hpp"      // for IMPLEMENT_COMPONENT_FACTORY
#include "gui/Desktop.hpp"               // for Desktop
#include "gui/Event.hpp"                 // for Event
#include "gui/Painter.hpp"               // for Painter
#include "gui/Paragraph.hpp"             // for Paragraph
#include "gui/Rect2D.hpp"                // for Rect2D
#include "gui/Signal.hpp"                // for Signal
#include "gui/Texture.hpp"               // for Texture
#include "gui/TextureManager.hpp"        // for TextureManager, texture_manager
#include "gui/XmlReader.hpp"             // for XmlReader
#include "libxml/xmlreader.h"            // for XML_READER_TYPE_ELEMENT
#include "lincity-ng/UserOperation.hpp"  // for UserOperation
#include "lincity/all_buildings.hpp"       // for GROUP_WATER_BUL_COST, GROUP_...
#include "lincity/commodities.hpp"       // for commodityNames
#include "lincity/groups.hpp"              // for GROUP_DESERT, GROUP_WATER
#include "lincity/lin-city.hpp"            // for FLAG_POWER_CABLES_0, FLAG_PO...
#include "lincity/lintypes.hpp"            // for ConstructionGroup, Construction
#include "lincity/messages.hpp"          // for Message
#include "lincity/modules/tile.hpp"      // for TileConstructionGroup, bareC...
#include "lincity/transport.hpp"           // for BRIDGE_FACTOR
#include "lincity/world.hpp"               // for Map, World, MapTile, Ground
#include "tinygettext/gettext.hpp"       // for _, dictionaryManager
#include "tinygettext/tinygettext.hpp"   // for Dictionary, DictionaryManager

using namespace std::placeholders;


const int scale3d = 128; // guestimate value for good looking 3d view;

const float GameView::defaultTileWidth = 128;
const float GameView::defaultTileHeight = 64;
const float GameView::defaultZoom = 1.0;    // fastest drawing

GameView::GameView() {
    loaderThread = 0;
    keyScrollState = SCROLL_NONE;
    mouseScrollState = SCROLL_NONE;
    remaining_images = 0;
    textures_ready = false;
}

GameView::~GameView() {
    stopThread = true;
    SDL_WaitThread( loaderThread, NULL );
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

    mapOverlay = overlayNone;
    mapMode = MiniMap::NORMAL;
    buttonsConnected = false;
    lastStatusMessage = "";
}

World&
GameView::getWorld() const {
  return game->getWorld();
}

UserOperation *
GameView::getUserOperation() const {
  return &game->getUserOperation();
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
    button->clicked.connect(std::bind(&GameView::buttonClicked, this, _1));

    button = getButton( *root, "showTerrainHeight" );
    button->clicked.connect(std::bind(&GameView::buttonClicked, this, _1));

    button = getButton( *root, "mapOverlay" );
    button->clicked.connect(std::bind(&GameView::buttonClicked, this, _1));
}

/*
 * Handle Clicks on Button.
 */
void GameView::buttonClicked( Button* button ){
    std::string name = button->getName();
    if( name == "hideHighBuildings" ){
        hideHigh = !hideHigh;
        setDirty();
        return;
    }
    if( name == "showTerrainHeight" ){
        if(getWorld().map.alt_step != 0) {
            showTerrainHeight = !showTerrainHeight;
        } else { // map is completely flat
            showTerrainHeight = false;
        }
        setDirty();
        return;
    }
    if( name == "mapOverlay" ){
        mapOverlay = (mapOverlay + 1) % (overlayMAX + 1);
        setDirty();
        return;
    }
    std::cerr << "GameView::buttonClicked# Unhandled Button '" << name <<"',\n";
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
void GameView::readOrigin( bool redraw /* = true */ ) {
  show(getWorld().map.recentPoint, redraw);
}

/*
 * set main_screen_originx and main_screen_originy
 */
void GameView::writeOrigin() {
  getWorld().map.recentPoint = getCenter().x;
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
        setDirty();
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
    if(fabs(newzoom - 1.0) < .01) {
      // fix rounding errors...
      newzoom = 1;
      factor = newzoom / zoom;
    }

    zoom = newzoom;

    tileWidth = defaultTileWidth * zoom;
    tileHeight = defaultTileHeight * zoom;

    viewport = (viewport + mousepos) * factor - mousepos;
    constrainViewportPosition(true);

    viewportUpdated();
    setDirty();
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
    center.x = ( map.x - map.y ) * ( tileWidth / 2 );
    center.y = ( map.x + map.y ) * ( tileHeight / 2 ) + ( tileHeight / 2 );
    if( redraw ){
        viewport.x = center.x - ( getWidth() / 2 );
        viewport.y = center.y - ( getHeight() / 2 );
        viewportUpdated();
        setDirty();
    } else { //on startup getWidth is 0.
        viewport.x = center.x - ( getConfig()->videoX / 2 );
        viewport.y = center.y - ( getConfig()->videoY / 2 );
    }
}

/*
 * Loads Texture from filename, Returns Pointer to Texture
 * or Null if no file found. NOT THREADSAFE
 */

Texture* GameView::readTexture(const std::filesystem::path& filename) {
  Texture* currentTexture;
  try {
    currentTexture = texture_manager->load(
      std::filesystem::path("images") / "tiles" / filename);
  } catch(std::exception& err) {
    std::cerr << filename << " GameView::readTexture# missing: "
      << err.what() << "\n";
    return 0;
  }
  return currentTexture;
}

/*
 * Loads Image from filename, Returns Pointer to Image
 * or Null if no file found. IS THREADSAFE
 */
SDL_Surface* GameView::readImage(const std::filesystem::path& filename) {
  SDL_Surface* currentImage = IMG_Load((getConfig()->appDataDir / "images" /
    "tiles" / filename).string().c_str());
  if( !currentImage ) {
    std::cerr << "GameView::readImage# Could not load image " << filename << "\n";
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
    XmlReader reader(getConfig()->appDataDir /
      "images" / "tiles" / "images.xml");

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

    viewportUpdated();
    setDirty();
}

bool GameView::constrainViewportPosition(bool useScrollCorrection) {
  //If the centre of the Screen is not Part of the city
  //adjust viewport so it is.
  if(useScrollCorrection)
    viewport += scrollCorrection * zoom;
  Vector2 center = viewport + (Vector2(getWidth(), getHeight()) / 2);
  Vector2 centerTile = Vector2(
    center.y / tileHeight + center.x / tileWidth,
    center.y / tileHeight - center.x / tileWidth
  );
  bool outside = false;
  int mapSize = getWorld().map.len();
  if(centerTile.x < 1) {
      centerTile.x = 1;
      outside = true;
  }
  else if(centerTile.x > mapSize - 1) {
      centerTile.x = mapSize - 1;
      outside = true;
  }
  if(centerTile.y < 1) {
      centerTile.y = 1;
      outside = true;
  }
  else if(centerTile.y > mapSize - 1) {
      centerTile.y = mapSize - 1;
      outside = true;
  }

  if(outside) {
      Vector2 vpOld = viewport;
      center.x = ( centerTile.x - centerTile.y ) * tileWidth / 2;
      center.y = ( centerTile.x + centerTile.y ) * tileHeight / 2;
      viewport = center - (Vector2(getWidth(), getHeight()) / 2);
      if(useScrollCorrection)
        scrollCorrection = (vpOld - viewport) / zoom;
      else
        scrollCorrection = Vector2(0,0);
      viewportUpdated();
      setDirty();
      return true;
  }
  else {
    scrollCorrection = Vector2(0,0);
    return false;
  }
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
                viewportUpdated();
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
            (getUserOperation()->action != UserOperation::ACTION_EVACUATE))
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

            if(roadDragging && !areaBulldoze
              && getUserOperation()->action == UserOperation::ACTION_BULLDOZE
            ) {
              if(tile != startRoad) {
                game->executeUserOperation(startRoad);
                startRoad = tile;
              }
            }

            if(tileUnderMouse != tile) {
                tileUnderMouse = tile;
                setDirty();
                //update mps target
                if(getUserOperation()->action == UserOperation::ACTION_EVACUATE)
                  game->getMpsMap().setTile(
                    game->getWorld().map.is_visible(tile)
                    ? game->getWorld().map(tile)
                    : nullptr
                  );
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
                if ( roadDragging && event.inside )
                {
                    MapPoint endRoad = getTile( event.mousepos );
                    roadDragging = false;
                    areaBulldoze = false;
                    leftButtonDown = false;
                    if( cursorSize != 1 ){//roadDragging was aborted with Escape
                        break;
                    }
                    if(blockingDialogIsOpen)
                      break;

                    //build last tile first to play the sound
                    Message::ptr dummyMsg;
                    // check if allowed to avoid many dialogs for bulk ops
                    if(getUserOperation()->isAllowedHere(
                      game->getWorld(), endRoad, dummyMsg)
                    ) {
                      game->executeUserOperation(endRoad);
                    }
                    MapPoint currentTile = startRoad;
                    int stepx = ( startRoad.x > endRoad.x ) ? -1 : 1;
                    int stepy = ( startRoad.y > endRoad.y ) ? -1 : 1;
                    if(getUserOperation()->action ==
                      UserOperation::ACTION_BULLDOZE
                    ) {
                      for(currentTile.x = startRoad.x;
                        currentTile.x != endRoad.x + stepx;
                        currentTile.x += stepx
                      )
                      for(currentTile.y = startRoad.y;
                        currentTile.y != endRoad.y + stepy;
                        currentTile.y += stepy
                      ) {
                        if(getUserOperation()->isAllowedHere(
                          game->getWorld(), currentTile, dummyMsg)
                        ) {
                          game->executeUserOperation(currentTile);
                        }
                      }
                    }
                    else if(getUserOperation()->action ==
                      UserOperation::ACTION_BUILD
                    ) {
                      int* v1 = ctrDrag ? &currentTile.y :&currentTile.x;
                      int* v2 = ctrDrag ? &currentTile.x :&currentTile.y;
                      int* l1 = ctrDrag ? &endRoad.y :&endRoad.x;
                      int* l2 = ctrDrag ? &endRoad.x :&endRoad.y;
                      int* s1 = ctrDrag ? &stepy: &stepx;
                      int* s2 = ctrDrag ? &stepx: &stepy;

                      while(*v1 != *l1) {
                        if(getUserOperation()->isAllowedHere(
                          game->getWorld(), currentTile, dummyMsg)
                        ) {
                          game->executeUserOperation(currentTile);
                        }
                        *v1 += *s1;
                      }
                      while(*v2 != *l2) {
                        if(getUserOperation()->isAllowedHere(
                          game->getWorld(), currentTile, dummyMsg)
                        ) {
                          game->executeUserOperation(currentTile);
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

            if(event.mousebutton == SDL_BUTTON_LEFT) {
              if(!blockingDialogIsOpen) {
                game->executeUserOperation(getTile(event.mousepos));
              }
            }
            else if(event.mousebutton == SDL_BUTTON_RIGHT) {
                // show info on the clicked thing
                MapPoint point = getTile(event.mousepos);
                if(!inCity(point)) break;
                game->getMpsMap().query(game->getWorld().map(point));
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

            if(event.keysym.scancode == SDL_SCANCODE_G) {
              MpsMap& mps = game->getMpsMap();
              if(mps.page == MpsMap::Page::GROUND)
                mps.page = MpsMap::Page::INVENTORY;
              else
                mps.page = MpsMap::Page::GROUND;
              mps.refresh();
              game->getMiniMap().switchView("MapMPS");
              break;
            }
            //Hide High Buildings
            if( event.keysym.scancode == SDL_SCANCODE_H ){
                if( hideHigh ){
                    hideHigh = false;
                } else {
                    hideHigh = true;
                }
                setDirty();
                break;
            }
            //overlay MiniMap Information
            if( event.keysym.scancode == SDL_SCANCODE_V ){
                mapOverlay = (mapOverlay+1)%(overlayMAX+1);
                setDirty();
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
                show(MapPoint(getWorld().map.len() / 2, getWorld().map.len() / 2));
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
    viewportUpdated();
    setDirty();
}

/*
 *  We should draw the whole City again.
 */
void GameView::viewportUpdated()
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
}

/*
 * Find point on Screen, where lower right corner of tile
 * is placed.
 */
Vector2 GameView::getScreenPoint(MapPoint mp)
{
    Vector2 point;
    point.x = (mp.x - mp.y) * ( tileWidth / 2 );
    point.y = (mp.x + mp.y) * ( tileHeight / 2 );

    //we want the lower right corner
    point.y += tileHeight;

    if ((showTerrainHeight) && (inCity(mp))){
        // shift the tile upward to show altitude
        point.y -= (float) ( (getWorld().map(mp)->ground.altitude) * scale3d) * zoom  / (float) getWorld().map.alt_step ;
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
    tile.x = (int) floorf(point.y / tileHeight + point.x / tileWidth);
    tile.y = (int) floorf(point.y / tileHeight - point.x / tileWidth);

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
    return getWorld().map.is_visible(tile);
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
        miniMapColor = getMiniMap()->getColor(tile);
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
MapPoint GameView::realTile(MapPoint point) {
  if(!inCity(point))
    return point;
  else if(getWorld().map(point)->reportingConstruction)
    return getWorld().map(point)->reportingConstruction->point;
  else
    return point;
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

    ConstructionGroup *cstgrp =
      getWorld().map(upperLeft)->getTopConstructionGroup();
    ResourceGroup *resgrp;
    unsigned short size = cstgrp->size;

    //Attention map is rotated for displaying
    if(tile.n(size-1) == upperLeft) //Signs are tested
    {
        resgrp = getWorld().map(upperLeft)->getTileResourceGroup();
        //adjust OnScreenPoint of big Tiles
        MapPoint lowerRightTile(tile.e(size-1));
        unsigned short textureType = getWorld().map(upperLeft)->getTopType();

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
        if( (getWorld().map(upperLeft)->reportingConstruction || getWorld().map(upperLeft)->framesptr) && !(getWorld().map(upperLeft)->flags & FLAG_INVISIBLE) )
        {
            if (resgrp->images_loaded)
            {
                size_t s = resgrp->graphicsInfoVector.size();
                if (s)
                {
                    graphicsInfo = &resgrp->graphicsInfoVector
                        [ getWorld().map(upperLeft)->type  % s];
                    drawTexture(painter, lowerRightTile, graphicsInfo);
                }
            }
        }
        bool draw_colored_site = true;

        if( (size==1 || !hideHigh) )
        {
            draw_colored_site = false;
            if (getWorld().map(upperLeft)->framesptr)
            {
                for(std::list<ExtraFrame>::iterator frit = getWorld().map(upperLeft)->framesptr->begin();
                    frit != getWorld().map(upperLeft)->framesptr->end(); std::advance(frit, 1))
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
            painter.setFillColor(getMiniMap()->getColorNormal(tile));
            fillDiamond( painter, tilerect );
        }
        //last draw suspended power cables on top
        //only works for size == 1
        if (getWorld().map(upperLeft)->flags & (FLAG_POWER_CABLES_0 | FLAG_POWER_CABLES_90))
        {
            resgrp = ResourceGroup::resMap["PowerLine"];
            if(resgrp->images_loaded)
            {
                if (getWorld().map(upperLeft)->flags & FLAG_POWER_CABLES_0)
                {   drawTexture(painter, upperLeft, &resgrp->graphicsInfoVector[23]);}
                if (getWorld().map(upperLeft)->flags & FLAG_POWER_CABLES_90)
                {   drawTexture(painter, upperLeft, &resgrp->graphicsInfoVector[22]);}
            }
        }
    }


}

/*
 * Mark a tile with current cursor
 */
void GameView::markTile(Painter& painter, MapPoint point) {
    Vector2 tileOnScreenPoint = getScreenPoint(point);
    {
      MapPoint upperLeft = realTile(point);
      Construction *cst;
      if(game->getMpsMap().tile
        && upperLeft == game->getMpsMap().tile->point
        && getUserOperation()->action == UserOperation::ACTION_QUERY
        && (cst = getWorld().map(point)->reportingConstruction)
      ) {
        ConstructionGroup *constructionGroup = cst->constructionGroup;
        int range = constructionGroup->range;
        int edgelen = 2 * range + constructionGroup->size;
        painter.setFillColor(Color(0, 255, 0, 64));
        Rect2D rangerect(0, 0, tileWidth * edgelen, tileHeight * edgelen);
        Vector2 screenPoint = getScreenPoint(upperLeft);
        screenPoint.x -= tileWidth * (0.5*edgelen);
        screenPoint.y -= tileHeight * (range + 1);
        rangerect.move(screenPoint);
        fillDiamond(painter, rangerect);
      }
    }

    if( getUserOperation()->action == UserOperation::ACTION_QUERY) //  cursorSize == 0
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
        Message::ptr tmpMsg;
        if(getUserOperation()->isAllowedHere(getWorld(), point, tmpMsg))
          painter.setFillColor(alphablue);
        else
          painter.setFillColor(alphared);

        Rect2D tilerect( 0, 0, tileWidth * cursorSize, tileHeight * cursorSize );
        tileOnScreenPoint.x -= (tileWidth * cursorSize / 2);
        tileOnScreenPoint.y -= tileHeight;
        tilerect.move( tileOnScreenPoint );
        fillDiamond( painter, tilerect );

        if(getUserOperation()->action == UserOperation::ACTION_BUILD)
        {
            // Draw range for selected_building
            int range = getUserOperation()->constructionGroup->range;
            if (range > 0 )
            {
                int edgelen = 2 * range + getUserOperation()->constructionGroup->size ;
                painter.setFillColor( Color( 0, 0, 128, 64 ) );
                Rect2D rangerect( 0,0,
                                  tileWidth  * ( edgelen) ,
                                  tileHeight * ( edgelen) );
                Vector2 screenPoint = getScreenPoint(point);
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
    if(getUserOperation()->action == UserOperation::ACTION_EVACUATE) {
      game->getMpsMap().refresh();
      game->getMiniMap().switchView("MapMPS");
    }
    //Mark Tile under Mouse // TODO: handle showTerrainHeight
    if(mouseInGameView && !blockingDialogIsOpen) {
        MapPoint lastRazed( -1,-1 );
        int tiles = 0;
        if( roadDragging && ( cursorSize == 1 ) &&
        (getUserOperation()->action == UserOperation::ACTION_BUILD || getUserOperation()->action == UserOperation::ACTION_BULLDOZE))
        {
            //use same method to find all Tiles as in GameView::event(const Event& event)
            int stepx = ( startRoad.x > tileUnderMouse.x ) ? -1 : 1;
            int stepy = ( startRoad.y > tileUnderMouse.y ) ? -1 : 1;
            currentTile = startRoad;

            if (getUserOperation()->action == UserOperation::ACTION_BULLDOZE)
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
            else if (getUserOperation()->action == UserOperation::ACTION_BUILD)
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
            if( (getUserOperation()->action == UserOperation::ACTION_BULLDOZE ) && realTile( currentTile ) != lastRazed ) {
                    cost += bulldozeCost( tileUnderMouse );
            } else {
                cost += buildCost( tileUnderMouse );
            }
        }
        std::stringstream prize;
        if( getUserOperation()->action == UserOperation::ACTION_BULLDOZE ){
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
        else if( getUserOperation()->action == UserOperation::ACTION_BUILD)
        {
            std::string buildingName =  getUserOperation()->constructionGroup->name;
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

    if( getUserOperation()->action == UserOperation::ACTION_QUERY ) //query
    {
        infotextstream << _("Query Tool: Show information about selected building.");
    }
    else if( getUserOperation()->action == UserOperation::ACTION_BULLDOZE ) //bulldoze
    {
        infotextstream << _("Bulldozer: remove building -price varies-");
    }
    else if( getUserOperation()->action == UserOperation::ACTION_BUILD )
    {
        infotextstream << getUserOperation()->constructionGroup->getName();
        infotextstream << _(": Cost to build ") << getUserOperation()->constructionGroup->getCosts(getWorld()) <<_("$");
        infotextstream << _(", to bulldoze ") << getUserOperation()->constructionGroup->bul_cost <<_("$") << ".";
        if( number > 1 ){
            infotextstream << _(" To build ") << number << _(" of them ");
            infotextstream << _("will cost about ") << number*getUserOperation()->constructionGroup->getCosts(getWorld()) << _("$") << "-";
        }
    }
    else if ( getUserOperation()->action == UserOperation::ACTION_EVACUATE )
    {
        infotextstream << _("Evacuation of commodities is for free.");
    }
    else if ( getUserOperation()->action == UserOperation::ACTION_FLOOD )
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

    if (!getWorld().map.is_visible(tile))
    {   return 0;}
    Construction *reportingConstruction = getWorld().map(tile)->reportingConstruction;
    if (reportingConstruction)
    {   return reportingConstruction->constructionGroup->bul_cost;}
    else
    {
        int group = getWorld().map(tile)->getGroup();
        if (group == GROUP_DESERT)
        {   return 0;}
        else if (group == GROUP_WATER)
        {   return GROUP_WATER_BUL_COST;}
        else
        {   return 1;}
    }
    return 0;
}

int GameView::buildCost(MapPoint tile) {
  Message::ptr tmp;
  if(!getUserOperation()->isAllowedHere(getWorld(), tile, tmp)
    || getUserOperation()->action == UserOperation::ACTION_QUERY
    || getUserOperation()->action == UserOperation::ACTION_EVACUATE
  )
    return 0;
  if(getUserOperation()->action == UserOperation::ACTION_BUILD) {
    if (getWorld().map(tile)->is_water()) //building a bridge
      return BRIDGE_FACTOR * getUserOperation()->constructionGroup->getCosts(getWorld());
    else //building on land
      return getUserOperation()->constructionGroup->getCosts(getWorld());
  }
  if(getUserOperation()->action == UserOperation::ACTION_FLOOD)
    return GROUP_WATER_COST;
  return 0;
}

//Register as Component
IMPLEMENT_COMPONENT_FACTORY(GameView)

/** @file lincity-ng/GameView.cpp */
