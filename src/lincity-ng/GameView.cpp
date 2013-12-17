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
#include <config.h>

#include "GameView.hpp"

#include "gui_interface/mps.h"
#include "gui/TextureManager.hpp"
#include "gui/Painter.hpp"
#include "gui/Rect2D.hpp"
#include "gui/Color.hpp"
#include "gui/ComponentFactory.hpp"
#include "gui/XmlReader.hpp"
#include "gui/Event.hpp"
#include "PhysfsStream/PhysfsSDL.hpp"
#include "gui/Paragraph.hpp"
#include "gui/Desktop.hpp"

#include "lincity/lintypes.h"
#include "lincity/lctypes.h"
#include "lincity/engglobs.h"
#include "lincity/modules/all_modules.h"
#include "lincity/engine.h"
#include "lincity/lin-city.h"

#include "Mps.hpp"
#include "MapEdit.hpp"
#include "MiniMap.hpp"
#include "Dialog.hpp"
#include "Config.hpp"
#include "ScreenInterface.hpp"
#include "Util.hpp"
#include "Debug.hpp"

#include <SDL_keysym.h>
#include <math.h>
#include <sstream>
#include <physfs.h>

#include "gui_interface/shared_globals.h"
#include "tinygettext/gettext.hpp"

#include "gui/callback/Callback.hpp"
#include "gui/Button.hpp"
#include "CheckButton.hpp"

const int scale3d = 128; // guestimate value for good looking 3d view;

const float GameView::defaultTileWidth = 128;
const float GameView::defaultTileHeight = 64;
const float GameView::defaultZoom = 1.0;    // fastest drawing

GameView* gameViewPtr = 0;

GameView* getGameView()
{
    return gameViewPtr;
}


GameView::GameView()
{
    assert(gameViewPtr == 0);
    gameViewPtr = this;
    loaderThread = 0;
    keyScrollState = 0;
    mouseScrollState = 0;
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
    loaderThread = SDL_CreateThread( gameViewThread, this );

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
            std::string s1("Minimap: traffic density: ");
            std::string s2 = commodityNames[getMiniMap()->getStuffID()];
            printStatusMessage( s1 + s2 );
        }
            break;
        case MiniMap::COMMODITIES:
        {
            std::string s1("Minimap: commodities: ");
            std::string s2 = commodityNames[getMiniMap()->getStuffID()];
            printStatusMessage( s1 + s2 );
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
    std::string dirsep = PHYSFS_getDirSeparator();
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
    std::string dirsep = PHYSFS_getDirSeparator();

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
    std::string dirsep = PHYSFS_getDirSeparator();
    //std::string xmlfile = std::string("images") + dirsep
    //+ std::string("tiles") + dirsep + std::string("images.xml");

    std::ostringstream os;
    os << "images" << dirsep << "tiles" << dirsep << "images.xml";
    std::string xmlfile = os.str();

    XmlReader reader( xmlfile );

    ConstructionGroup *constructionGroup = 0;
    int resourceID_level = 0;
    int xmlX = -1;
    int xmlY = -1;
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
                        if(constructionGroup)
                        {
                            //std::cout << constructionGroup->name << " images loaded: line491" << std::endl;
                            constructionGroup->images_loaded = true;
                        }

                        if(ConstructionGroup::resourceMap.count(value))
                        {
                            constructionGroup = ConstructionGroup::resourceMap[value];
                            resourceID_level = reader.getDepth();
                            if(constructionGroup->images_loaded)
                            {
                                std::cout << "Duplicate resourceID in images.xml: " << value << std::endl;
                                assert(false);
                            }
                        }
                        else
                        {
                            std::cout << "unknown resourceID: " << value << " in images.xml" << std::endl;
                            constructionGroup = 0;
                            resourceID_level = 0;
                        }
                    }
                }
            }
            //check if we are still inside context of last resorceID
            if(reader.getDepth() < resourceID_level-1)
            {
                //std::cout << constructionGroup->name << " images loaded: line517" << std::endl;
                constructionGroup->images_loaded = true;
                constructionGroup = 0;
                resourceID_level = 0;
            }
            if( element == "image" )
            {
                XmlReader::AttributeIterator iter(reader);
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
                            xmlX = -1;
                        }
                    }
                   else if(strcmp(name, "y") == 0 )
                    {
                        if(sscanf(value, "%i", &xmlY) != 1)
                        {
                            std::cerr << "GameView::preReadCityXY# Error parsing integer value '" << value << "' in y attribute.\n";
                            xmlY = -1;
                        }
                    }
                }

                if (resourceID_level && constructionGroup)
                {

                    //std::cout << "Parsing: " << constructionGroup->name << " as " <<
                    //key << " x= " << xmlX << " y= " << xmlY << std::endl;
                    //SDL_mutexP( mTextures );
                    constructionGroup->growGraphicsInfoVector();
                    GraphicsInfo *graphicsInfo = &(constructionGroup->graphicsInfoVector.back());
                    graphicsInfo->image = readImage( key );
                    if(!graphicsInfo->image)
                    {
                        std::cout << "image error: " << key << std::endl;
                    }
                    if(xmlX == -1)
                    {   xmlX = int(graphicsInfo->image->w/2);}
                    if(xmlY == -1)
                    {   xmlY = int(graphicsInfo->image->h);}
                    graphicsInfo->x = xmlX;
                    graphicsInfo->y = xmlY;
                    ++remaining_images;
                    //SDL_mutexV( mTextures );
                }

                xmlX = -1;
                xmlY = -1;
                key.clear();
            }
        }
    }
    if(constructionGroup)
    {
        //std::cout << constructionGroup->name << " images loaded: line580" << std::endl;
        constructionGroup->images_loaded = true;
        constructionGroup = 0;
    }
}

/*
void GameView::preReadCityTexture( int textureType, const std::string& filename )
{
    //skip loading if we stop anyway
    if(stopThread) {
        return;
    }

    int xmlX = -1;
    int xmlY = -1;

    SDL_mutexP( mTextures );
    cityImages[ textureType ] = readImage( filename );
    if( cityImages[ textureType ] )
    {
        //now we need to find x and y for our filename
        if(cityXmap.count(filename))
        {   xmlX = cityXmap[filename];}
        else
        {   xmlX = int( cityImages[ textureType ]->w / 2 );}

        if(cityYmap.count(filename))
        {   xmlY = cityYmap[filename];}
        else
        {   xmlY = int( cityImages[ textureType ]->h );}
#ifdef DEBUG
        assert(xmlX > 0 && xmlY > 0);
#endif
        cityTextureX[ textureType ] = xmlX;
        cityTextureY[ textureType ] = xmlY;
    }
    SDL_mutexV( mTextures );
}
*/

/**
 *  Preload all required Textures. (his Function is called by loaderThread)
 *  Some of the Image to Texture Conversion seems not to be threadsave
 *  in OpenGL, so load just Images and convert them to Textures on
 *  demand in the main Tread.
 */
/*
void GameView::loadTextures()
{
   //We need Textures for all Types from lincity/lctypes.h
   //Code Generation:
*/
   /*
       grep -e LCT src/lincity/lctypes.h | sed  \
           -e 's/#define LC/   preReadCityTexture( CS/' \
           -e 's/_G /, /'       \
           -e 's/_G\t/, /'     \
           -e 's/"/.png" );/2'
   */
/*
   //preReadCityTexture( CST_GREEN,   "green.png" );
   //preReadCityTexture( CST_DESERT_0,    "desert_0.png" );
   //preReadCityTexture( CST_DESERT_1L,   "desert_1l.png" );
   //preReadCityTexture( CST_DESERT_1R,   "desert_1r.png" );
   //preReadCityTexture( CST_DESERT_1U,   "desert_1u.png" );
   //preReadCityTexture( CST_DESERT_1D,   "desert_1d.png" );
   //preReadCityTexture( CST_DESERT_2LR,  "desert_2lr.png" );
   //preReadCityTexture( CST_DESERT_2LU,  "desert_2lu.png" );
   //preReadCityTexture( CST_DESERT_2LD,  "desert_2ld.png" );
   //preReadCityTexture( CST_DESERT_2RU,  "desert_2ru.png" );
   //preReadCityTexture( CST_DESERT_2RD,  "desert_2rd.png" );
   //preReadCityTexture( CST_DESERT_2UD,  "desert_2ud.png" );
   //preReadCityTexture( CST_DESERT_3LRD, "desert_3lrd.png" );
   //preReadCityTexture( CST_DESERT_3LRU, "desert_3lru.png" );
   //preReadCityTexture( CST_DESERT_3LUD, "desert_3lud.png" );
   //preReadCityTexture( CST_DESERT_3RUD, "desert_3rud.png" );
   //preReadCityTexture( CST_DESERT,  "desert.png" );
   //preReadCityTexture( CST_TREE,    "tree.png" );
   //preReadCityTexture( CST_TREE2,   "tree2.png" );
   //preReadCityTexture( CST_TREE3,   "tree3.png" );
   //preReadCityTexture( CST_POWERL_H_L, "powerlhl.png" );
   //preReadCityTexture( CST_POWERL_V_L,      "powerlvl.png" );
   //preReadCityTexture( CST_POWERL_LD_L,     "powerlldl.png" );
   //preReadCityTexture( CST_POWERL_RD_L,     "powerlrdl.png" );
   //preReadCityTexture( CST_POWERL_LU_L,     "powerllul.png" );
   //preReadCityTexture( CST_POWERL_RU_L,     "powerlrul.png" );
   //preReadCityTexture( CST_POWERL_LDU_L, "powerlldul.png" );
   //preReadCityTexture( CST_POWERL_LDR_L, "powerlldrl.png" );
   //preReadCityTexture( CST_POWERL_LUR_L, "powerllurl.png" );
   //preReadCityTexture( CST_POWERL_UDR_L, "powerludrl.png" );
   //preReadCityTexture( CST_POWERL_LUDR_L, "powerlludrl.png" );
   //preReadCityTexture( CST_POWERL_H_D,        "powerlhd.png" );
   //preReadCityTexture( CST_POWERL_V_D,        "powerlvd.png" );
   //preReadCityTexture( CST_POWERL_LD_D,       "powerlldd.png" );
   //preReadCityTexture( CST_POWERL_RD_D,       "powerlrdd.png" );
   //preReadCityTexture( CST_POWERL_LU_D,       "powerllud.png" );
   //preReadCityTexture( CST_POWERL_RU_D,       "powerlrud.png" );
   //preReadCityTexture( CST_POWERL_LDU_D,      "powerlldud.png" );
   //preReadCityTexture( CST_POWERL_LDR_D,      "powerlldrd.png" );
   //preReadCityTexture( CST_POWERL_LUR_D,      "powerllurd.png" );
   //preReadCityTexture( CST_POWERL_UDR_D,      "powerludrd.png" );
   //preReadCityTexture( CST_POWERL_LUDR_D,     "powerlludrd.png" );
   //preReadCityTexture( CST_SHANTY,            "shanty.png" );
   //preReadCityTexture( CST_POWERS_SOLAR, "powerssolar.png" );
   //preReadCityTexture( CST_POWERS_COAL_EMPTY, "powerscoal-empty.png" );
   //preReadCityTexture( CST_POWERS_COAL_LOW,   "powerscoal-low.png" );
   //preReadCityTexture( CST_POWERS_COAL_MED,   "powerscoal-med.png" );
   //preReadCityTexture( CST_POWERS_COAL_FULL,  "powerscoal-full.png" );
   //preReadCityTexture( CST_BURNT,   "burnt_land.png" );
   //preReadCityTexture( CST_SUBSTATION_R, "substation-R.png" );
   //preReadCityTexture( CST_SUBSTATION_G,      "substation-G.png" );
   //preReadCityTexture( CST_SUBSTATION_RG,     "substation-RG.png" );
   //preReadCityTexture( CST_UNIVERSITY, "university.png" );
   //preReadCityTexture( CST_RESIDENCE_LL, "reslowlow.png" );
   //preReadCityTexture( CST_RESIDENCE_ML, "resmedlow.png" );
   //preReadCityTexture( CST_RESIDENCE_HL, "reshilow.png" );
   //preReadCityTexture( CST_RESIDENCE_LH,      "reslowhi.png" );
   //preReadCityTexture( CST_RESIDENCE_MH,      "resmedhi.png" );
   //preReadCityTexture( CST_RESIDENCE_HH,      "reshihi.png" );
   //preReadCityTexture( CST_MARKET_EMPTY, "market-empty.png" );
   //preReadCityTexture( CST_MARKET_LOW,        "market-low.png" );
   //preReadCityTexture( CST_MARKET_MED,        "market-med.png" );
   //preReadCityTexture( CST_MARKET_FULL,       "market-full.png" );
   //preReadCityTexture( CST_RECYCLE,     "recycle-centre.png" );
   //preReadCityTexture( CST_TRACK_LR,    "tracklr.png" );
   //preReadCityTexture( CST_TRACK_LU,          "tracklu.png" );
   //preReadCityTexture( CST_TRACK_LD,          "trackld.png" );
   //preReadCityTexture( CST_TRACK_UD,          "trackud.png" );
   //preReadCityTexture( CST_TRACK_UR,          "trackur.png" );
   //preReadCityTexture( CST_TRACK_DR,          "trackdr.png" );
   //preReadCityTexture( CST_TRACK_LUR,         "tracklur.png" );
   //preReadCityTexture( CST_TRACK_LDR,         "trackldr.png" );
   //preReadCityTexture( CST_TRACK_LUD,         "tracklud.png" );
   //preReadCityTexture( CST_TRACK_UDR,         "trackudr.png" );
   //preReadCityTexture( CST_TRACK_LUDR,        "trackludr.png" );
   //preReadCityTexture( CST_PARKLAND_PLANE, "parkland-plane.png" );
   //preReadCityTexture( CST_PARKLAND_LAKE, "parkland-lake.png" );
   //preReadCityTexture( CST_MONUMENT_0, "monument0.png" );
   //preReadCityTexture( CST_MONUMENT_1,        "monument1.png" );
   //preReadCityTexture( CST_MONUMENT_2,        "monument2.png" );
   //preReadCityTexture( CST_MONUMENT_3,        "monument3.png" );
   //preReadCityTexture( CST_MONUMENT_4,        "monument4.png" );
   //preReadCityTexture( CST_MONUMENT_5,        "monument5.png" );
   //preReadCityTexture( CST_COALMINE_EMPTY, "coalmine-empty.png" );
   //preReadCityTexture( CST_COALMINE_LOW, "coalmine-low.png" );
   //preReadCityTexture( CST_COALMINE_MED, "coalmine-med.png" );
   //preReadCityTexture( CST_COALMINE_FULL, "coalmine-full.png" );
   //preReadCityTexture( CST_RAIL_LR,          "raillr.png" );
   //preReadCityTexture( CST_RAIL_LU,          "raillu.png" );
   //preReadCityTexture( CST_RAIL_LD,          "railld.png" );
   //preReadCityTexture( CST_RAIL_UD,          "railud.png" );
   //preReadCityTexture( CST_RAIL_UR,          "railur.png" );
   //preReadCityTexture( CST_RAIL_DR,          "raildr.png" );
   //preReadCityTexture( CST_RAIL_LUR,         "raillur.png" );
   //preReadCityTexture( CST_RAIL_LDR,         "railldr.png" );
   //preReadCityTexture( CST_RAIL_LUD,         "raillud.png" );
   //preReadCityTexture( CST_RAIL_UDR,         "railudr.png" );
   //preReadCityTexture( CST_RAIL_LUDR,        "railludr.png" );
   //preReadCityTexture( CST_FIRE_1,           "fire1.png" );
   //preReadCityTexture( CST_FIRE_2,           "fire2.png" );
   //preReadCityTexture( CST_FIRE_3,           "fire3.png" );
   //preReadCityTexture( CST_FIRE_4,           "fire4.png" );
   //preReadCityTexture( CST_FIRE_5,           "fire5.png" );
   //preReadCityTexture( CST_FIRE_DONE1,       "firedone1.png" );
   //preReadCityTexture( CST_FIRE_DONE2,       "firedone2.png" );
   //preReadCityTexture( CST_FIRE_DONE3,       "firedone3.png" );
   //preReadCityTexture( CST_FIRE_DONE4,       "firedone4.png" );
   //preReadCityTexture( CST_ROAD_LR,          "roadlr.png" );
   //preReadCityTexture( CST_ROAD_LU,          "roadlu.png" );
   //preReadCityTexture( CST_ROAD_LD,          "roadld.png" );
   //preReadCityTexture( CST_ROAD_UD,          "roadud.png" );
   //preReadCityTexture( CST_ROAD_UR,          "roadur.png" );
   //preReadCityTexture( CST_ROAD_DR,          "roaddr.png" );
   //preReadCityTexture( CST_ROAD_LUR,         "roadlur.png" );
   //preReadCityTexture( CST_ROAD_LDR,         "roadldr.png" );
   //preReadCityTexture( CST_ROAD_LUD,         "roadlud.png" );
   //preReadCityTexture( CST_ROAD_UDR,         "roadudr.png" );
   //preReadCityTexture( CST_ROAD_LUDR,        "roadludr.png" );
   //preReadCityTexture( CST_OREMINE_5,         "oremine5.png" );
   //preReadCityTexture( CST_OREMINE_6,         "oremine6.png" );
   //preReadCityTexture( CST_OREMINE_7,         "oremine7.png" );
   //preReadCityTexture( CST_OREMINE_8,         "oremine8.png" );
   //preReadCityTexture( CST_OREMINE_1,   "oremine1.png" );
   //preReadCityTexture( CST_OREMINE_2,   "oremine2.png" );
   //preReadCityTexture( CST_OREMINE_3,   "oremine3.png" );
   //preReadCityTexture( CST_OREMINE_4,   "oremine4.png" );
   //preReadCityTexture( CST_HEALTH,  "health.png" );
   //preReadCityTexture( CST_SCHOOL,  "school0.png" );
   //preReadCityTexture( CST_EX_PORT,     "ex_port.png" );
   //preReadCityTexture( CST_MILL_0,            "mill0.png" );
   //preReadCityTexture( CST_MILL_1,            "mill1.png" );
   //preReadCityTexture( CST_MILL_2,            "mill2.png" );
   //preReadCityTexture( CST_MILL_3,            "mill3.png" );
   //preReadCityTexture( CST_MILL_4,            "mill4.png" );
   //preReadCityTexture( CST_MILL_5,            "mill5.png" );
   //preReadCityTexture( CST_MILL_6,            "mill6.png" );
   //preReadCityTexture( CST_ROCKET_1,          "rocket1.png" );
   //preReadCityTexture( CST_ROCKET_2,    "rocket2.png" );
   //preReadCityTexture( CST_ROCKET_3,    "rocket3.png" );
   //preReadCityTexture( CST_ROCKET_4,    "rocket4.png" );
   //preReadCityTexture( CST_ROCKET_5,          "rocket5.png" );
   //preReadCityTexture( CST_ROCKET_6,          "rocket6.png" );
   //preReadCityTexture( CST_ROCKET_7,    "rocket7.png" );
   //preReadCityTexture( CST_ROCKET_FLOWN, "rocketflown.png" );
   //preReadCityTexture( CST_WINDMILL_1_G,      "windmill1g.png" );
   //preReadCityTexture( CST_WINDMILL_2_G,      "windmill2g.png" );
   //preReadCityTexture( CST_WINDMILL_3_G,      "windmill3g.png" );
   //preReadCityTexture( CST_WINDMILL_1_RG,     "windmill1rg.png" );
   //preReadCityTexture( CST_WINDMILL_2_RG,     "windmill2rg.png" );
   //preReadCityTexture( CST_WINDMILL_3_RG,     "windmill3rg.png" );
   //preReadCityTexture( CST_WINDMILL_1_R,      "windmill1r.png" );
   //preReadCityTexture( CST_WINDMILL_2_R,      "windmill2r.png" );
   //preReadCityTexture( CST_WINDMILL_3_R,      "windmill3r.png" );
   //preReadCityTexture( CST_WINDMILL_1_W,      "windmill1w.png" );
   //preReadCityTexture( CST_WINDMILL_2_W,      "windmill2w.png" );
   //preReadCityTexture( CST_WINDMILL_3_W,      "windmill3w.png" );
   //preReadCityTexture( CST_BLACKSMITH_0,        "blacksmith0.png" );
   //preReadCityTexture( CST_BLACKSMITH_1,        "blacksmith1.png" );
   //preReadCityTexture( CST_BLACKSMITH_2,        "blacksmith2.png" );
   //preReadCityTexture( CST_BLACKSMITH_3,        "blacksmith3.png" );
   //preReadCityTexture( CST_BLACKSMITH_4,        "blacksmith4.png" );
   //preReadCityTexture( CST_BLACKSMITH_5,        "blacksmith5.png" );
   //preReadCityTexture( CST_BLACKSMITH_6,        "blacksmith6.png" );
   //preReadCityTexture( CST_POTTERY_0,           "pottery0.png" );
   //preReadCityTexture( CST_POTTERY_1,           "pottery1.png" );
   //preReadCityTexture( CST_POTTERY_2,           "pottery2.png" );
   //preReadCityTexture( CST_POTTERY_3,           "pottery3.png" );
   //preReadCityTexture( CST_POTTERY_4,           "pottery4.png" );
   //preReadCityTexture( CST_POTTERY_5,           "pottery5.png" );
   //preReadCityTexture( CST_POTTERY_6,           "pottery6.png" );
   //preReadCityTexture( CST_POTTERY_7,           "pottery7.png" );
   //preReadCityTexture( CST_POTTERY_8,           "pottery8.png" );
   //preReadCityTexture( CST_POTTERY_9,           "pottery9.png" );
   //preReadCityTexture( CST_POTTERY_10,          "pottery10.png" );
   //preReadCityTexture( CST_WATER,             "water.png" );
   //preReadCityTexture( CST_WATER_D,           "waterd.png" );
   //preReadCityTexture( CST_WATER_R,           "waterr.png" );
   //preReadCityTexture( CST_WATER_U,           "wateru.png" );
   //preReadCityTexture( CST_WATER_L,           "waterl.png" );
   //preReadCityTexture( CST_WATER_LR,          "waterlr.png" );
   //preReadCityTexture( CST_WATER_UD,          "waterud.png" );
   //preReadCityTexture( CST_WATER_LD,          "waterld.png" );
   //preReadCityTexture( CST_WATER_RD,          "waterrd.png" );
   //preReadCityTexture( CST_WATER_LU,          "waterlu.png" );
   //preReadCityTexture( CST_WATER_UR,          "waterur.png" );
   //preReadCityTexture( CST_WATER_LUD,         "waterlud.png" );
   //preReadCityTexture( CST_WATER_LRD,         "waterlrd.png" );
   //preReadCityTexture( CST_WATER_LUR,         "waterlur.png" );
   //preReadCityTexture( CST_WATER_URD,         "waterurd.png" );
   //preReadCityTexture( CST_WATER_LURD,        "waterlurd.png" );
   //preReadCityTexture( CST_WATERWELL,         "waterwell.png" );
   //preReadCityTexture( CST_CRICKET_1,         "cricket1.png" );
   //preReadCityTexture( CST_CRICKET_2,         "cricket2.png" );
   //preReadCityTexture( CST_CRICKET_3,         "cricket3.png" );
   //preReadCityTexture( CST_CRICKET_4,         "cricket4.png" );
   //preReadCityTexture( CST_CRICKET_5,         "cricket5.png" );
   //preReadCityTexture( CST_CRICKET_6,         "cricket6.png" );
   //preReadCityTexture( CST_CRICKET_7,         "cricket7.png" );
   //preReadCityTexture( CST_FIRESTATION_1,       "firestation1.png" );
   //preReadCityTexture( CST_FIRESTATION_2,       "firestation2.png" );
   //preReadCityTexture( CST_FIRESTATION_3,       "firestation3.png" );
   //preReadCityTexture( CST_FIRESTATION_4,       "firestation4.png" );
   //preReadCityTexture( CST_FIRESTATION_5,       "firestation5.png" );
   //preReadCityTexture( CST_FIRESTATION_6,       "firestation6.png" );
   //preReadCityTexture( CST_FIRESTATION_7,       "firestation7.png" );
   //preReadCityTexture( CST_FIRESTATION_8,       "firestation8.png" );
   //preReadCityTexture( CST_FIRESTATION_9,       "firestation9.png" );
   //preReadCityTexture( CST_FIRESTATION_10,      "firestation10.png" );
   //preReadCityTexture( CST_TIP_0,             "tip0.png" );
   //preReadCityTexture( CST_TIP_1,             "tip1.png" );
   //preReadCityTexture( CST_TIP_2,             "tip2.png" );
   //preReadCityTexture( CST_TIP_3,             "tip3.png" );
   //preReadCityTexture( CST_TIP_4,             "tip4.png" );
   //preReadCityTexture( CST_TIP_5,             "tip5.png" );
   //preReadCityTexture( CST_TIP_6,             "tip6.png" );
   //preReadCityTexture( CST_TIP_7,             "tip7.png" );
   //preReadCityTexture( CST_TIP_8,             "tip8.png" );
   //preReadCityTexture( CST_COMMUNE_1,         "commune1.png" );
   //preReadCityTexture( CST_COMMUNE_2,         "commune2.png" );
   //preReadCityTexture( CST_COMMUNE_3,         "commune3.png" );
   //preReadCityTexture( CST_COMMUNE_4,         "commune4.png" );
   //preReadCityTexture( CST_COMMUNE_5,         "commune5.png" );
   //preReadCityTexture( CST_COMMUNE_6,         "commune6.png" );
   //preReadCityTexture( CST_COMMUNE_7,         "commune7.png" );
   //preReadCityTexture( CST_COMMUNE_8,         "commune8.png" );
   //preReadCityTexture( CST_COMMUNE_9,         "commune9.png" );
   //preReadCityTexture( CST_COMMUNE_10,        "commune10.png" );
   //preReadCityTexture( CST_COMMUNE_11,        "commune11.png" );
   //preReadCityTexture( CST_COMMUNE_12,        "commune12.png" );
   //preReadCityTexture( CST_COMMUNE_13,        "commune13.png" );
   //preReadCityTexture( CST_COMMUNE_14,        "commune14.png" );
   //preReadCityTexture( CST_INDUSTRY_H_C,      "industryhc.png" );
   //preReadCityTexture( CST_INDUSTRY_H_L1,      "industryhl1.png" );
   //preReadCityTexture( CST_INDUSTRY_H_L2,      "industryhl2.png" );
   //preReadCityTexture( CST_INDUSTRY_H_L3,      "industryhl3.png" );
   //preReadCityTexture( CST_INDUSTRY_H_L4,      "industryhl4.png" );
   //preReadCityTexture( CST_INDUSTRY_H_L5,      "industryhl5.png" );
   //preReadCityTexture( CST_INDUSTRY_H_L6,      "industryhl6.png" );
   //preReadCityTexture( CST_INDUSTRY_H_L7,      "industryhl7.png" );
   //preReadCityTexture( CST_INDUSTRY_H_L8,      "industryhl8.png" );
   //preReadCityTexture( CST_INDUSTRY_H_M1,      "industryhm1.png" );
   //preReadCityTexture( CST_INDUSTRY_H_M2,      "industryhm2.png" );
   //preReadCityTexture( CST_INDUSTRY_H_M3,      "industryhm3.png" );
   //preReadCityTexture( CST_INDUSTRY_H_M4,      "industryhm4.png" );
   //preReadCityTexture( CST_INDUSTRY_H_M5,      "industryhm5.png" );
   //preReadCityTexture( CST_INDUSTRY_H_M6,      "industryhm6.png" );
   //preReadCityTexture( CST_INDUSTRY_H_M7,      "industryhm7.png" );
   //preReadCityTexture( CST_INDUSTRY_H_M8,      "industryhm8.png" );
   //preReadCityTexture( CST_INDUSTRY_H_H1,      "industryhh1.png" );
   //preReadCityTexture( CST_INDUSTRY_H_H2,      "industryhh2.png" );
   //preReadCityTexture( CST_INDUSTRY_H_H3,      "industryhh3.png" );
   //preReadCityTexture( CST_INDUSTRY_H_H4,      "industryhh4.png" );
   //preReadCityTexture( CST_INDUSTRY_H_H5,      "industryhh5.png" );
   //preReadCityTexture( CST_INDUSTRY_H_H6,      "industryhh6.png" );
   //preReadCityTexture( CST_INDUSTRY_H_H7,      "industryhh7.png" );
   //preReadCityTexture( CST_INDUSTRY_H_H8,      "industryhh8.png" );
   //preReadCityTexture( CST_INDUSTRY_L_C,       "industrylc.png" );
   //preReadCityTexture( CST_INDUSTRY_L_Q1,      "industrylq1.png" );
   //preReadCityTexture( CST_INDUSTRY_L_Q2,      "industrylq2.png" );
   //preReadCityTexture( CST_INDUSTRY_L_Q3,      "industrylq3.png" );
   //preReadCityTexture( CST_INDUSTRY_L_Q4,      "industrylq4.png" );
   //preReadCityTexture( CST_INDUSTRY_L_L1,      "industryll1.png" );
   //preReadCityTexture( CST_INDUSTRY_L_L2,      "industryll2.png" );
   //preReadCityTexture( CST_INDUSTRY_L_L3,      "industryll3.png" );
   //preReadCityTexture( CST_INDUSTRY_L_L4,      "industryll4.png" );
   //preReadCityTexture( CST_INDUSTRY_L_M1,      "industrylm1.png" );
   //preReadCityTexture( CST_INDUSTRY_L_M2,      "industrylm2.png" );
   //preReadCityTexture( CST_INDUSTRY_L_M3,      "industrylm3.png" );
   //preReadCityTexture( CST_INDUSTRY_L_M4,      "industrylm4.png" );
   //preReadCityTexture( CST_INDUSTRY_L_H1,      "industrylh1.png" );
   //preReadCityTexture( CST_INDUSTRY_L_H2,      "industrylh2.png" );
   //preReadCityTexture( CST_INDUSTRY_L_H3,      "industrylh3.png" );
   //preReadCityTexture( CST_INDUSTRY_L_H4,      "industrylh4.png" );
   //preReadCityTexture( CST_FARM_O0,            "farm0.png" );
   //preReadCityTexture( CST_FARM_O1,            "farm1.png" );
   //preReadCityTexture( CST_FARM_O2,            "farm2.png" );
   //preReadCityTexture( CST_FARM_O3,            "farm3.png" );
   //preReadCityTexture( CST_FARM_O4,            "farm4.png" );
   //preReadCityTexture( CST_FARM_O5,            "farm5.png" );
   //preReadCityTexture( CST_FARM_O6,            "farm6.png" );
   //preReadCityTexture( CST_FARM_O7,            "farm7.png" );
   //preReadCityTexture( CST_FARM_O8,            "farm8.png" );
   //preReadCityTexture( CST_FARM_O9,            "farm9.png" );
   //preReadCityTexture( CST_FARM_O10,           "farm10.png" );
   //preReadCityTexture( CST_FARM_O11,           "farm11.png" );
   //preReadCityTexture( CST_FARM_O12,           "farm12.png" );
   //preReadCityTexture( CST_FARM_O13,           "farm13.png" );
   //preReadCityTexture( CST_FARM_O14,           "farm14.png" );
   //preReadCityTexture( CST_FARM_O15,           "farm15.png" );
   //preReadCityTexture( CST_FARM_O16,           "farm16.png" );
   //preReadCityTexture( CST_TRACK_BRIDGE_LR,    "Trackbridge2.png" );
   //preReadCityTexture( CST_TRACK_BRIDGE_UD,    "Trackbridge1.png" );
   //preReadCityTexture( CST_TRACK_BRIDGE_LRP,   "Trackbridge_pg1.png" );
   //preReadCityTexture( CST_TRACK_BRIDGE_UDP,   "Trackbridge_pg2.png" );
   //preReadCityTexture( CST_TRACK_BRIDGE_ILR,   "Trackbridge_entrance_270.png" );
   //preReadCityTexture( CST_TRACK_BRIDGE_OLR,   "Trackbridge_entrance_90.png" );
   //preReadCityTexture( CST_TRACK_BRIDGE_IUD,   "Trackbridge_entrance_00.png" );
   //preReadCityTexture( CST_TRACK_BRIDGE_OUD,   "Trackbridge_entrance_180.png" );
   //preReadCityTexture( CST_ROAD_BRIDGE_LR,     "Roadbridge1.png" );
   //preReadCityTexture( CST_ROAD_BRIDGE_UD,     "Roadbridge2.png" );
   //preReadCityTexture( CST_ROAD_BRIDGE_LRP,    "Roadbridge1s.png" );
   //preReadCityTexture( CST_ROAD_BRIDGE_UDP,    "Roadbridge2s.png" );
   //preReadCityTexture( CST_ROAD_BRIDGE_LRPG,   "Roadbridge_pg1.png" );
   //preReadCityTexture( CST_ROAD_BRIDGE_UDPG,   "Roadbridge_pg2.png" );
   //preReadCityTexture( CST_ROAD_BRIDGE_I1LR,   "Roadbridge_entrance1_270.png" );
   //preReadCityTexture( CST_ROAD_BRIDGE_O1LR,   "Roadbridge_entrance1_90.png" );
   //preReadCityTexture( CST_ROAD_BRIDGE_I1UD,   "Roadbridge_entrance1_00.png" );
   //preReadCityTexture( CST_ROAD_BRIDGE_O1UD,   "Roadbridge_entrance1_180.png" );
   //preReadCityTexture( CST_ROAD_BRIDGE_I2LR,   "Roadbridge_entrance2_270.png" );
   //preReadCityTexture( CST_ROAD_BRIDGE_O2LR,   "Roadbridge_entrance2_90.png" );
   //preReadCityTexture( CST_ROAD_BRIDGE_I2UD,   "Roadbridge_entrance2_00.png" );
   //preReadCityTexture( CST_ROAD_BRIDGE_O2UD,   "Roadbridge_entrance2_180.png" );
   //preReadCityTexture( CST_RAIL_BRIDGE_LR,     "Railbridge1.png" );
   //preReadCityTexture( CST_RAIL_BRIDGE_UD,     "Railbridge2.png" );
   //preReadCityTexture( CST_RAIL_BRIDGE_LRPG,   "Railbridge_pg1.png" );
   //preReadCityTexture( CST_RAIL_BRIDGE_UDPG,   "Railbridge_pg2.png" );
   //preReadCityTexture( CST_RAIL_BRIDGE_I1LR,   "Railbridge_entrance1_270.png" );
   //preReadCityTexture( CST_RAIL_BRIDGE_O1LR,   "Railbridge_entrance1_90.png" );
   //preReadCityTexture( CST_RAIL_BRIDGE_I1UD,   "Railbridge_entrance1_00.png" );
   //preReadCityTexture( CST_RAIL_BRIDGE_O1UD,   "Railbridge_entrance1_180.png" );
   //preReadCityTexture( CST_RAIL_BRIDGE_I2LR,   "Railbridge_entrance2_270.png" );
   //preReadCityTexture( CST_RAIL_BRIDGE_O2LR,   "Railbridge_entrance2_90.png" );
   //preReadCityTexture( CST_RAIL_BRIDGE_I2UD,   "Railbridge_entrance2_00.png" );
   //preReadCityTexture( CST_RAIL_BRIDGE_O2UD,   "Railbridge_entrance2_180.png" );

   // End of generated Code.
}
*/

/*
 * Scroll the map.
 */
void GameView::scroll( void )
{
    static Uint32 oldTime = SDL_GetTicks();
    Uint32 now = SDL_GetTicks();
    //TODO: scroll speed should be configurable
    float stepx = (now - oldTime) * tileWidth / 100;
    float stepy = (now - oldTime) * tileHeight / 100;
    oldTime = now;

    if( keyScrollState == 0 && mouseScrollState == 0 ) {
        return;
    }

    if( keyScrollState & (SCROLL_LSHIFT | SCROLL_RSHIFT) ) {
        stepx *= 4;
        stepy *= 4;
    }

    if( (keyScrollState | mouseScrollState) &
            (SCROLL_UP | SCROLL_UP_LEFT | SCROLL_UP_RIGHT) ) {
        viewport.y -= stepy;
    }
    if( (keyScrollState | mouseScrollState) &
            (SCROLL_DOWN | SCROLL_DOWN_LEFT | SCROLL_DOWN_RIGHT) ) {
        viewport.y += stepy;
    }
    if( (keyScrollState | mouseScrollState) &
            (SCROLL_LEFT | SCROLL_UP_LEFT | SCROLL_DOWN_LEFT) ) {
        viewport.x -= stepx;
    }
    if( (keyScrollState | mouseScrollState) &
            (SCROLL_RIGHT | SCROLL_UP_RIGHT | SCROLL_DOWN_RIGHT) ) {
        viewport.x += stepx;
    }
   requestRedraw();
}

/*
 * Process event
 */
void GameView::event(const Event& event)
{
    switch(event.type) {
        case Event::MOUSEMOTION: {
            mouseScrollState = 0;
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

            if( dragging ) {
                if(fabsf(event.mousemove.x) < 1 && fabsf(event.mousemove.y) < 1)
                    break;
                // this was most probably a SDL_WarpMouse
                if(event.mousepos == dragStart)
                    break;
                viewport += event.mousemove;
                setDirty();
                break;
            }
            if(!event.inside) {
                mouseInGameView = false;
                break;
            }
            mouseInGameView = true;

            if( !dragging && rightButtonDown ) {
                dragging = true;
                dragStart = event.mousepos;
                SDL_ShowCursor( SDL_DISABLE );
                dragStartTime = SDL_GetTicks();
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
                if( tile != startRoad ){
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
            if( event.mousebutton == SDL_BUTTON_RIGHT ) {
                dragging = false;
                ctrDrag = false;
                rightButtonDown = true;
                break;
            }
            if( event.mousebutton == SDL_BUTTON_LEFT ) {
                roadDragging = false;
                areaBulldoze = false;
                ctrDrag = false;
                leftButtonDown = true;
                break;
            }
            if( event.mousebutton == SDL_BUTTON_MIDDLE ) {
                if( inCity( getTile( event.mousepos ) ) ) {
                    getMiniMap()->showMpsEnv( getTile( event.mousepos ) );
                }
            }
            break;
        }
        case Event::MOUSEBUTTONUP:

            if(event.mousebutton == SDL_BUTTON_MIDDLE ){
                getMiniMap()->hideMpsEnv();
            }

            if( event.mousebutton == SDL_BUTTON_RIGHT ){
                if ( dragging ) {
                    dragging = false;
                    rightButtonDown = false;
                    SDL_ShowCursor( SDL_ENABLE );
                    SDL_WarpMouse((Uint16) dragStart.x, (Uint16) dragStart.y);
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
                        while( *v1 != *l1 )
                        {
                            if(userOperation->is_allowed_here(currentTile.x, currentTile.y, false))
                            {   place_item(currentTile.x, currentTile.y);}
                            *v1 += *s1;
                        }

                        while( *v2 != *l2 )
                        {
                            if(userOperation->is_allowed_here(currentTile.x, currentTile.y, false))
                            {   place_item(currentTile.x, currentTile.y);}
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
                recenter(event.mousepos);                               //adjust view
            }
            else if( event.mousebutton == SDL_BUTTON_WHEELUP ){         //up
                recenter(event.mousepos);                               //adjust view
                SDL_WarpMouse((Uint16) getWidth() / 2, (Uint16) getHeight() / 2);//set mouse to center
                zoomIn();                                               //zoom in
            }
            else if( event.mousebutton == SDL_BUTTON_WHEELDOWN ){       //down
                recenter(event.mousepos);                               //adjust view
                SDL_WarpMouse((Uint16) getWidth() / 2, (Uint16) getHeight() / 2);//set mouse to center
                zoomOut();                                              //zoom out
            }
            break;
        case Event::KEYDOWN:
            if( event.keysym.sym == SDLK_LCTRL || event.keysym.sym == SDLK_RCTRL ){
                if (roadDragging)
                {   ctrDrag = !ctrDrag;}
                break;
            }
            if( event.keysym.sym == SDLK_KP8 || event.keysym.sym == SDLK_UP ){
                keyScrollState |= SCROLL_UP;
                break;
            }
            if( event.keysym.sym == SDLK_KP2 || event.keysym.sym == SDLK_DOWN ){
                keyScrollState |= SCROLL_DOWN;
                break;
            }
            if( event.keysym.sym == SDLK_KP4 || event.keysym.sym == SDLK_LEFT ){
                keyScrollState |= SCROLL_LEFT;
                break;
            }
            if( event.keysym.sym == SDLK_KP6 || event.keysym.sym == SDLK_RIGHT ){
                keyScrollState |= SCROLL_RIGHT;
                break;
            }
            if( event.keysym.sym == SDLK_KP7 ){
                keyScrollState |= SCROLL_UP_LEFT;
                break;
            }
            if( event.keysym.sym == SDLK_KP9 ){
                keyScrollState |= SCROLL_UP_RIGHT;
                break;
            }
            if( event.keysym.sym == SDLK_KP1 ){
                keyScrollState |= SCROLL_DOWN_LEFT;
                break;
            }
            if( event.keysym.sym == SDLK_KP3 ){
                keyScrollState |= SCROLL_DOWN_RIGHT;
                break;
            }
            if( event.keysym.sym == SDLK_LSHIFT ){
                keyScrollState |= SCROLL_LSHIFT;
                break;
            }
            if( event.keysym.sym == SDLK_RSHIFT ){
                keyScrollState |= SCROLL_RSHIFT;
                break;
            }


            // use G to show ground info aka MpsEnv without middle mouse button
            if( event.keysym.sym == SDLK_g){
                if( inCity(tileUnderMouse) ) {
                    getMiniMap()->showMpsEnv( tileUnderMouse );
                }
                break;
            }
            // hotkeys for scrolling pages up and down
           if(event.keysym.sym == SDLK_n)
            {
                getMiniMap()->scrollPageDown(true);
                break;
            }
            if (event.keysym.sym == SDLK_m)
            {
                getMiniMap()->scrollPageDown(false);
                break;
            }
            break;
        case Event::KEYUP:
/*
            //TEst
            if( event.keysym.sym == SDLK_x ){
                writeOrigin();
                readOrigin();
                break;
            }
*/
            //Hide High Buildings
            if( event.keysym.sym == SDLK_h ){
                if( hideHigh ){
                    hideHigh = false;
                } else {
                    hideHigh = true;
                }
                requestRedraw();
                break;
            }
            //overlay MiniMap Information
            if( event.keysym.sym == SDLK_v ){
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
            if( event.keysym.sym == SDLK_KP_ENTER || event.keysym.sym == SDLK_RETURN){
                resetZoom();
                break;
            }
            //Scroll
            if( event.keysym.sym == SDLK_KP8 || event.keysym.sym == SDLK_UP ){
                keyScrollState &= ~SCROLL_UP;
                break;
            }
            if( event.keysym.sym == SDLK_KP2 || event.keysym.sym == SDLK_DOWN ){
                keyScrollState &= ~SCROLL_DOWN;
                break;
            }
            if( event.keysym.sym == SDLK_KP4 || event.keysym.sym == SDLK_LEFT ){
                keyScrollState &= ~SCROLL_LEFT;
                break;
            }
            if( event.keysym.sym == SDLK_KP6 || event.keysym.sym == SDLK_RIGHT ){
                keyScrollState &= ~SCROLL_RIGHT;
                break;
            }
            if( event.keysym.sym == SDLK_KP7 ){
                keyScrollState &= ~SCROLL_UP_LEFT;
                break;
            }
            if( event.keysym.sym == SDLK_KP9 ){
                keyScrollState &= ~SCROLL_UP_RIGHT;
                break;
            }
            if( event.keysym.sym == SDLK_KP1 ){
                keyScrollState &= ~SCROLL_DOWN_LEFT;
                break;
            }
            if( event.keysym.sym == SDLK_KP3 ){
                keyScrollState &= ~SCROLL_DOWN_RIGHT;
                break;
            }
            if( event.keysym.sym == SDLK_LSHIFT ){
                keyScrollState &= ~SCROLL_LSHIFT;
                break;
            }
            if( event.keysym.sym == SDLK_RSHIFT ){
                keyScrollState &= ~SCROLL_RSHIFT;
                break;
            }

            if ( event.keysym.sym == SDLK_KP5 ) {
                show(MapPoint(world.len() / 2, world.len() / 2));
                setDirty();
                break;
            }
            break;

        case Event::UPDATE:
            if(dragging)
                SDL_WarpMouse((Uint16) dragStart.x, (Uint16) dragStart.y);
            break;

        default:
            break;
    }
}

/*
 * Parent tells us to change size.
 */
void GameView::resize(float newwidth , float newheight )
{
    width = newwidth;
    height = newheight;
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
    std::map<std::string, ConstructionGroup*>::iterator it;
    for(it= ConstructionGroup::resourceMap.begin(); it != ConstructionGroup::resourceMap.end(); ++it)
    {
        for(size_t i = 0; i < it->second->graphicsInfoVector.size(); ++i)
        {
            if( !it->second->graphicsInfoVector[i].texture) // && !economyGraph_open
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

    ConstructionGroup *cstgrp = world(upperLeft.x, upperLeft.y)->getTopConstructionGroup();
    unsigned short size = cstgrp->size;

    //Attention map is rotated for displaying
    if ( ( tile.x != x ) || ( tile.y - size +1 != y ) ) //Signs are tested
    {   return;}

    //adjust OnScreenPoint of big Tiles
    MapPoint lowerRightTile( tile.x + size - 1 , tile.y );
    unsigned short textureType = world(upperLeft.x, upperLeft.y)->getTopType();

    GraphicsInfo *graphicsInfo = 0;
    //draw terrain underneath special constructions
    if (cstgrp == &powerlineConstructionGroup)
    {
        ConstructionGroup *tilegrp = world(upperLeft.x, upperLeft.y)->getTileConstructionGroup();
        if (tilegrp->images_loaded)
        {
            size_t s = tilegrp->graphicsInfoVector.size();
            if (s)
            {
                graphicsInfo = &tilegrp->graphicsInfoVector
                [ world(x, y)->type  % s];
                drawTexture(painter, lowerRightTile, graphicsInfo);
            }
        }
    }

    // if we hide high buildings, hide trees as well
    if (hideHigh && (cstgrp == &treeConstructionGroup
     || cstgrp == &tree2ConstructionGroup
     || cstgrp == &tree3ConstructionGroup ))
    {   cstgrp = &bareConstructionGroup;}

    if(cstgrp->images_loaded && (size==1 || !hideHigh))
    {
        size_t s = cstgrp->graphicsInfoVector.size();
        if (s)
        {
            graphicsInfo = &cstgrp->graphicsInfoVector[ textureType % s];
            drawTexture(painter, lowerRightTile, graphicsInfo);
        }
    }
    else
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
    if(powerlineConstructionGroup.images_loaded)
    {
        x = lowerRightTile.x;
        y = lowerRightTile.y;
        cstgrp = &powerlineConstructionGroup;
        if (world(x, y)->flags & FLAG_POWER_CABLES_0)
        {   drawTexture(painter, lowerRightTile, &cstgrp->graphicsInfoVector[23]);}
        if (world(x, y)->flags & FLAG_POWER_CABLES_90)
        {   drawTexture(painter, lowerRightTile, &cstgrp->graphicsInfoVector[22]);}
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
    //If the centre of the Screen is not Part of the city
    //adjust viewport so it is.
    MapPoint centerTile = getCenter();
    bool outside = false;
    if( centerTile.x < gameAreaMin )
    {
        centerTile.x = gameAreaMin;
        outside = true;
    }
    if( centerTile.x > gameAreaMax() )
    {
        centerTile.x = gameAreaMax();
        outside = true;
    }
    if( centerTile.y < gameAreaMin )
    {
        centerTile.y = gameAreaMin;
        outside = true;
    }
    if( centerTile.y > gameAreaMax() )
    {
        centerTile.y = gameAreaMax();
        outside = true;
    }
    if( outside )
    {
        mouseScrollState = 0;   //Avoid clipping in pause mode
        keyScrollState = 0;
        show( centerTile );
        return;
    }


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
    Color green;
    Rect2D background( 0, 0, getWidth(), getHeight() );
    green.parse( "green" );
    painter.setFillColor( green );
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

            if ( (userOperation->action == UserOperation::ACTION_BULLDOZE))
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
        std::string buildingName =  userOperation->constructionGroup->name;
        infotextstream << dictionaryManager->get_dictionary().translate( buildingName );
        infotextstream << _(": Cost to build ") << userOperation->constructionGroup->getCosts() <<_("$");
        infotextstream << _(", to bulldoze ") << userOperation->constructionGroup->bul_cost <<_("$.");
        if( number > 1 ){
            infotextstream << _(" To build ") << number << _(" of them ");
            infotextstream << _("will cost about ") << number*userOperation->constructionGroup->getCosts() << _("$.");
        }
    }
    else if ( userOperation->action == UserOperation::ACTION_EVACUATE )
    {
        infotextstream << "Evacuation of commodities is for free.";
    }
    else if ( userOperation->action == UserOperation::ACTION_FLOOD )
    {
        infotextstream << "Water: Cost to build " << GROUP_WATER_COST << "$";
        infotextstream << ", to bulldoze " << GROUP_WATER_BUL_COST << "$.";
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

