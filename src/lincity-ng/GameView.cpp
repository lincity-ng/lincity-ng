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

#include "lincity/lin-city.h"
#include "lincity/lctypes.h"
#include "lincity/engglobs.h"

#include "MapEdit.hpp"
#include "MiniMap.hpp"
#include "Dialog.hpp"
#include "Config.hpp"
#include "ScreenInterface.hpp"
#include "Util.hpp"

#include <SDL_keysym.h>
#include <math.h>
#include <sstream>
#include <physfs.h>

#include "gui_interface/shared_globals.h"
#include "tinygettext/gettext.hpp"

#include "gui/callback/Callback.hpp"
#include "gui/Button.hpp"
#include "CheckButton.hpp"

extern int is_allowed_here(int x, int y, short cst_type, short msg);

const float GameView::defaultTileWidth = 128;
const float GameView::defaultTileHeight = 64;
const float GameView::defaultZoom = 0.5;

GameView* gameViewPtr = 0;

GameView* getGameView()
{
    return gameViewPtr;
}
    

GameView::GameView()
{
    assert(gameViewPtr == 0);
    gameViewPtr = this;
    mTextures = SDL_CreateMutex();
    mThreadRunning = SDL_CreateMutex();
    loaderThread = 0;
    keyScrollState = 0;
    mouseScrollState = 0;
}

GameView::~GameView()
{
    stopThread = true;
    SDL_mutexP( mThreadRunning );
    //SDL_KillThread( loaderThread );
    SDL_WaitThread( loaderThread, NULL );

    SDL_DestroyMutex( mThreadRunning );
    SDL_DestroyMutex( mTextures );
    
    for(int i = 0; i < NUM_OF_TYPES; ++i) {
        delete cityTextures[i];
        // in case the image was loaded but no texture created yet we have a
        // dangling SDL_Surface here
        SDL_FreeSurface(cityImages[i]);
    }

    if(gameViewPtr == this)
        gameViewPtr = 0;
}

//Static function to use with SDL_CreateThread
int GameView::gameViewThread( void* data )
{
    GameView* gv = (GameView*) data;
    gv->loadTextures();
    gv->requestRedraw();
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

    //Load Textures
    blankTexture = readTexture( "blank.png" );
    memset( &cityTextures, 0, sizeof( cityTextures ) );
    memset( &cityImages, 0, sizeof( cityImages ) );
    stopThread = false;
    SDL_mutexP( mThreadRunning );
    loaderThread = SDL_CreateThread( gameViewThread, this );
    SDL_mutexV( mThreadRunning );
   
    //GameView is resizable
    setFlags(FLAG_RESIZABLE);

    //start at location from savegame
    zoom = defaultZoom;
    tileWidth = defaultTileWidth * zoom;
    tileHeight = defaultTileHeight * zoom; 
    virtualScreenWidth = tileWidth * WORLD_SIDE_LEN;
    virtualScreenHeight = tileHeight * WORLD_SIDE_LEN;
    readOrigin( false );

    mouseInGameView = false;
    dragging = false;
	leftButtonDown = false;
    roadDragging = false;
    startRoad = MapPoint(0, 0);
    rightButtonDown = false;
    tileUnderMouse = MapPoint(0, 0);
    dragStart = Vector2(0, 0);
    hideHigh = false;
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
    if( name == "mapOverlay" ){
        mapOverlay++;
        if( mapOverlay > overlayMAX ) {
            mapOverlay = overlayNone;
        }
        requestRedraw();
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
            printStatusMessage( _("Minimap: coal depots") );
            break;
        case MiniMap::TRAFFIC: 
            printStatusMessage( _("Minimap: traffic density") );
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
    
    if ( newzoom < .0625 ) return;
    if ( newzoom > 4 ) return;
    
    zoom = newzoom;
    
    // fix rounding errors...
    if(fabs(zoom - 1.0) < .01)
        zoom = 1;
    
    tileWidth = defaultTileWidth * zoom;
    tileHeight = defaultTileHeight * zoom;
    //a virtual screen containing the whole city
    virtualScreenWidth = tileWidth * WORLD_SIDE_LEN;
    virtualScreenHeight = tileHeight * WORLD_SIDE_LEN;
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
 * or Null if no file found.
 */
Texture* GameView::readTexture(const std::string& filename)
{
    std::string nfilename = std::string("images/tiles/") + filename;
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
 * or Null if no file found.
 */
SDL_Surface* GameView::readImage(const std::string& filename)
{
    std::string nfilename = std::string("images/tiles/") + filename;
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
 * preload a City Texture and fill in X and Y Data.
 *
 * images/tiles/images.xml contains the x-Coordinate of the 
 * middle of the Building in Case the Image is asymetric,
 * eg. a high tower with a long shadow to the right 
 * 
 *  Some of the Image to Texture Conversion seems not to be threadsave
 *  in OpenGL, so load just Images and convert them to Textures on 
 *  demand in the main Tread. 
 */
void GameView::preReadCityTexture( int textureType, const std::string& filename )
{
    //skip loading if we stop anyway
    if(stopThread) {
        return;
    }
    XmlReader reader( "images/tiles/images.xml" );
    int xmlX = -1;
    int xmlY = -1;
    bool hit = false;
    
    SDL_mutexP( mTextures );
    cityImages[ textureType ] = readImage( filename );
    if( cityImages[ textureType ] ) 
    {
        //now we need to find x for our filename in images/tiles/images.xml 

        while( reader.read() ) 
        {
            if( reader.getNodeType() == XML_READER_TYPE_ELEMENT) 
            {
                const std::string& element = (const char*) reader.getName();
                if( element == "image" )
                {
                    XmlReader::AttributeIterator iter(reader);
                    while(iter.next()) 
                    {
                        const char* name = (const char*) iter.getName();
                        const char* value = (const char*) iter.getValue();
                        if( strcmp(name, "file" ) == 0 ) 
                        {
                            if( filename.compare( value ) == 0 ) 
                            {
                                hit = true;
                            }
                        }
                        else if( strcmp(name, "x" ) == 0 )
                        {
                            if(sscanf(value, "%i", &xmlX) != 1) 
                            {
                                std::cerr << "GameView::preReadCityTexture# Error parsing integer value '" << value << "' in x attribute.\n";
                                xmlX = -1;
                            }
                        }
                       else if(strcmp(name, "y") == 0 ) 
                        {
                            if(sscanf(value, "%i", &xmlY) != 1) 
                            {
                                std::cerr << "GameView::preReadCityTexture# Error parsing integer value '" << value << "' in y attribute.\n";
                                xmlY = -1;
                            }
                        }
                    } 
                    if( hit )
                    {
                        break;
                    }
                }
            }
        } 
       
        if( hit && ( xmlX >= 0 ) )
        { 
            cityTextureX[ textureType ] = xmlX;
        }
        else
        {
            cityTextureX[ textureType ] = int( ( cityImages[ textureType ]->w / 2 ) );
        }
        cityTextureY[ textureType ] = int( cityImages[ textureType ]->h ); 
    }
    SDL_mutexV( mTextures );
}
    
/**
 *  Preload all required Textures. (his Function is called by loaderThread)
 *  Some of the Image to Texture Conversion seems not to be threadsave
 *  in OpenGL, so load just Images and convert them to Textures on 
 *  demand in the main Tread. 
 */
void GameView::loadTextures()
{
   //We need Textures for all Types from lincity/lctypes.h 
   //Code Generation:
   /*
       grep -e LCT src/lincity/lctypes.h | sed  \
           -e 's/#define LC/   preReadCityTexture( CS/' \
           -e 's/_G /, /'       \
           -e 's/_G\t/, /'     \
           -e 's/"/.png" );/2'                 
   */
   preReadCityTexture( CST_GREEN, 	"green.png" );
   preReadCityTexture( CST_DESERT_0, 	"desert_0.png" );
   preReadCityTexture( CST_DESERT_1L, 	"desert_1l.png" );
   preReadCityTexture( CST_DESERT_1R, 	"desert_1r.png" );
   preReadCityTexture( CST_DESERT_1U, 	"desert_1u.png" );
   preReadCityTexture( CST_DESERT_1D, 	"desert_1d.png" );
   preReadCityTexture( CST_DESERT_2LR, 	"desert_2lr.png" );
   preReadCityTexture( CST_DESERT_2LU, 	"desert_2lu.png" );
   preReadCityTexture( CST_DESERT_2LD, 	"desert_2ld.png" );
   preReadCityTexture( CST_DESERT_2RU, 	"desert_2ru.png" );
   preReadCityTexture( CST_DESERT_2RD, 	"desert_2rd.png" );
   preReadCityTexture( CST_DESERT_2UD, 	"desert_2ud.png" );
   preReadCityTexture( CST_DESERT_3LRD,	"desert_3lrd.png" );
   preReadCityTexture( CST_DESERT_3LRU,	"desert_3lru.png" );
   preReadCityTexture( CST_DESERT_3LUD,	"desert_3lud.png" );
   preReadCityTexture( CST_DESERT_3RUD,	"desert_3rud.png" );
   preReadCityTexture( CST_DESERT, 	"desert.png" );
   preReadCityTexture( CST_TREE, 	"tree.png" );
   preReadCityTexture( CST_TREE2, 	"tree2.png" );
   preReadCityTexture( CST_TREE3, 	"tree3.png" );
   preReadCityTexture( CST_POWERL_H_L, "powerlhl.png" );
   preReadCityTexture( CST_POWERL_V_L,  	"powerlvl.png" );
   preReadCityTexture( CST_POWERL_LD_L, 	"powerlldl.png" );
   preReadCityTexture( CST_POWERL_RD_L, 	"powerlrdl.png" );
   preReadCityTexture( CST_POWERL_LU_L,  	"powerllul.png" );
   preReadCityTexture( CST_POWERL_RU_L,  	"powerlrul.png" );
   preReadCityTexture( CST_POWERL_LDU_L, "powerlldul.png" );
   preReadCityTexture( CST_POWERL_LDR_L, "powerlldrl.png" );
   preReadCityTexture( CST_POWERL_LUR_L, "powerllurl.png" );
   preReadCityTexture( CST_POWERL_UDR_L, "powerludrl.png" );
   preReadCityTexture( CST_POWERL_LUDR_L, "powerlludrl.png" );
   preReadCityTexture( CST_POWERL_H_D,        "powerlhd.png" );
   preReadCityTexture( CST_POWERL_V_D,        "powerlvd.png" );
   preReadCityTexture( CST_POWERL_LD_D,       "powerlldd.png" );
   preReadCityTexture( CST_POWERL_RD_D,       "powerlrdd.png" );
   preReadCityTexture( CST_POWERL_LU_D,       "powerllud.png" );
   preReadCityTexture( CST_POWERL_RU_D,       "powerlrud.png" );
   preReadCityTexture( CST_POWERL_LDU_D,      "powerlldud.png" );
   preReadCityTexture( CST_POWERL_LDR_D,      "powerlldrd.png" );
   preReadCityTexture( CST_POWERL_LUR_D,      "powerllurd.png" );
   preReadCityTexture( CST_POWERL_UDR_D,      "powerludrd.png" );
   preReadCityTexture( CST_POWERL_LUDR_D,     "powerlludrd.png" );
   preReadCityTexture( CST_SHANTY,            "shanty.png" );
   preReadCityTexture( CST_POWERS_SOLAR, "powerssolar.png" );
   preReadCityTexture( CST_POWERS_COAL_EMPTY, "powerscoal-empty.png" );
   preReadCityTexture( CST_POWERS_COAL_LOW,   "powerscoal-low.png" );
   preReadCityTexture( CST_POWERS_COAL_MED,   "powerscoal-med.png" );
   preReadCityTexture( CST_POWERS_COAL_FULL,  "powerscoal-full.png" );
   preReadCityTexture( CST_BURNT, 	"burnt_land.png" );
   preReadCityTexture( CST_SUBSTATION_R, "substation-R.png" );
   preReadCityTexture( CST_SUBSTATION_G,      "substation-G.png" );
   preReadCityTexture( CST_SUBSTATION_RG,     "substation-RG.png" );
   preReadCityTexture( CST_UNIVERSITY, "university.png" );
   preReadCityTexture( CST_RESIDENCE_LL, "reslowlow.png" );
   preReadCityTexture( CST_RESIDENCE_ML, "resmedlow.png" );
   preReadCityTexture( CST_RESIDENCE_HL, "reshilow.png" );
   preReadCityTexture( CST_RESIDENCE_LH,      "reslowhi.png" );
   preReadCityTexture( CST_RESIDENCE_MH,      "resmedhi.png" );
   preReadCityTexture( CST_RESIDENCE_HH,      "reshihi.png" );
   preReadCityTexture( CST_MARKET_EMPTY, "market-empty.png" );
   preReadCityTexture( CST_MARKET_LOW,        "market-low.png" );
   preReadCityTexture( CST_MARKET_MED,        "market-med.png" );
   preReadCityTexture( CST_MARKET_FULL,       "market-full.png" );
   preReadCityTexture( CST_RECYCLE, 	"recycle-centre.png" );
   preReadCityTexture( CST_TRACK_LR, 	"tracklr.png" );
   preReadCityTexture( CST_TRACK_LU,          "tracklu.png" );
   preReadCityTexture( CST_TRACK_LD,          "trackld.png" );
   preReadCityTexture( CST_TRACK_UD,          "trackud.png" );
   preReadCityTexture( CST_TRACK_UR,          "trackur.png" );
   preReadCityTexture( CST_TRACK_DR,          "trackdr.png" );
   preReadCityTexture( CST_TRACK_LUR,         "tracklur.png" );
   preReadCityTexture( CST_TRACK_LDR,         "trackldr.png" );
   preReadCityTexture( CST_TRACK_LUD,         "tracklud.png" );
   preReadCityTexture( CST_TRACK_UDR,         "trackudr.png" );
   preReadCityTexture( CST_TRACK_LUDR,        "trackludr.png" );
   preReadCityTexture( CST_PARKLAND_PLANE, "parkland-plane.png" );
   preReadCityTexture( CST_PARKLAND_LAKE, "parkland-lake.png" );
   preReadCityTexture( CST_MONUMENT_0, "monument0.png" );
   preReadCityTexture( CST_MONUMENT_1,        "monument1.png" );
   preReadCityTexture( CST_MONUMENT_2,        "monument2.png" );
   preReadCityTexture( CST_MONUMENT_3,        "monument3.png" );
   preReadCityTexture( CST_MONUMENT_4,        "monument4.png" );
   preReadCityTexture( CST_MONUMENT_5,        "monument5.png" );
   preReadCityTexture( CST_COALMINE_EMPTY, "coalmine-empty.png" );
   preReadCityTexture( CST_COALMINE_LOW, "coalmine-low.png" );
   preReadCityTexture( CST_COALMINE_MED, "coalmine-med.png" );
   preReadCityTexture( CST_COALMINE_FULL, "coalmine-full.png" );
   preReadCityTexture( CST_RAIL_LR,          "raillr.png" );
   preReadCityTexture( CST_RAIL_LU,          "raillu.png" );
   preReadCityTexture( CST_RAIL_LD,          "railld.png" );
   preReadCityTexture( CST_RAIL_UD,          "railud.png" );
   preReadCityTexture( CST_RAIL_UR,          "railur.png" );
   preReadCityTexture( CST_RAIL_DR,          "raildr.png" );
   preReadCityTexture( CST_RAIL_LUR,         "raillur.png" );
   preReadCityTexture( CST_RAIL_LDR,         "railldr.png" );
   preReadCityTexture( CST_RAIL_LUD,         "raillud.png" );
   preReadCityTexture( CST_RAIL_UDR,         "railudr.png" );
   preReadCityTexture( CST_RAIL_LUDR,        "railludr.png" );
   preReadCityTexture( CST_FIRE_1,           "fire1.png" );
   preReadCityTexture( CST_FIRE_2,           "fire2.png" );
   preReadCityTexture( CST_FIRE_3,           "fire3.png" );
   preReadCityTexture( CST_FIRE_4,           "fire4.png" );
   preReadCityTexture( CST_FIRE_5,           "fire5.png" );
   preReadCityTexture( CST_FIRE_DONE1,       "firedone1.png" );
   preReadCityTexture( CST_FIRE_DONE2,       "firedone2.png" );
   preReadCityTexture( CST_FIRE_DONE3,       "firedone3.png" );
   preReadCityTexture( CST_FIRE_DONE4,       "firedone4.png" );
   preReadCityTexture( CST_ROAD_LR,          "roadlr.png" );
   preReadCityTexture( CST_ROAD_LU,          "roadlu.png" );
   preReadCityTexture( CST_ROAD_LD,          "roadld.png" );
   preReadCityTexture( CST_ROAD_UD,          "roadud.png" );
   preReadCityTexture( CST_ROAD_UR,          "roadur.png" );
   preReadCityTexture( CST_ROAD_DR,          "roaddr.png" );
   preReadCityTexture( CST_ROAD_LUR,         "roadlur.png" );
   preReadCityTexture( CST_ROAD_LDR,         "roadldr.png" );
   preReadCityTexture( CST_ROAD_LUD,         "roadlud.png" );
   preReadCityTexture( CST_ROAD_UDR,         "roadudr.png" );
   preReadCityTexture( CST_ROAD_LUDR,        "roadludr.png" );
   preReadCityTexture( CST_OREMINE_5,         "oremine5.png" );
   preReadCityTexture( CST_OREMINE_6,         "oremine6.png" );
   preReadCityTexture( CST_OREMINE_7,         "oremine7.png" );
   preReadCityTexture( CST_OREMINE_8,         "oremine8.png" );
   preReadCityTexture( CST_OREMINE_1, 	"oremine1.png" );
   preReadCityTexture( CST_OREMINE_2, 	"oremine2.png" );
   preReadCityTexture( CST_OREMINE_3, 	"oremine3.png" );
   preReadCityTexture( CST_OREMINE_4, 	"oremine4.png" );
   preReadCityTexture( CST_HEALTH, 	"health.png" );
   preReadCityTexture( CST_SCHOOL, 	"school0.png" );
   preReadCityTexture( CST_EX_PORT, 	"ex_port.png" );
   preReadCityTexture( CST_MILL_0,            "mill0.png" );
   preReadCityTexture( CST_MILL_1,            "mill1.png" );
   preReadCityTexture( CST_MILL_2,            "mill2.png" );
   preReadCityTexture( CST_MILL_3,            "mill3.png" );
   preReadCityTexture( CST_MILL_4,            "mill4.png" );
   preReadCityTexture( CST_MILL_5,            "mill5.png" );
   preReadCityTexture( CST_MILL_6,            "mill6.png" );
   preReadCityTexture( CST_ROCKET_1,          "rocket1.png" );
   preReadCityTexture( CST_ROCKET_2, 	"rocket2.png" );
   preReadCityTexture( CST_ROCKET_3, 	"rocket3.png" );
   preReadCityTexture( CST_ROCKET_4, 	"rocket4.png" );
   preReadCityTexture( CST_ROCKET_5,          "rocket5.png" );
   preReadCityTexture( CST_ROCKET_6,          "rocket6.png" );
   preReadCityTexture( CST_ROCKET_7, 	"rocket7.png" );
   preReadCityTexture( CST_ROCKET_FLOWN, "rocketflown.png" );
   preReadCityTexture( CST_WINDMILL_1_G,      "windmill1g.png" );
   preReadCityTexture( CST_WINDMILL_2_G,      "windmill2g.png" );
   preReadCityTexture( CST_WINDMILL_3_G,      "windmill3g.png" );
   preReadCityTexture( CST_WINDMILL_1_RG,     "windmill1rg.png" );
   preReadCityTexture( CST_WINDMILL_2_RG,     "windmill2rg.png" );
   preReadCityTexture( CST_WINDMILL_3_RG,     "windmill3rg.png" );
   preReadCityTexture( CST_WINDMILL_1_R,      "windmill1r.png" );
   preReadCityTexture( CST_WINDMILL_2_R,      "windmill2r.png" );
   preReadCityTexture( CST_WINDMILL_3_R,      "windmill3r.png" );
   preReadCityTexture( CST_WINDMILL_1_W,      "windmill1w.png" );
   preReadCityTexture( CST_WINDMILL_2_W,      "windmill2w.png" );
   preReadCityTexture( CST_WINDMILL_3_W,      "windmill3w.png" );
   preReadCityTexture( CST_BLACKSMITH_0,        "blacksmith0.png" );
   preReadCityTexture( CST_BLACKSMITH_1,        "blacksmith1.png" );
   preReadCityTexture( CST_BLACKSMITH_2,        "blacksmith2.png" );
   preReadCityTexture( CST_BLACKSMITH_3,        "blacksmith3.png" );
   preReadCityTexture( CST_BLACKSMITH_4,        "blacksmith4.png" );
   preReadCityTexture( CST_BLACKSMITH_5,        "blacksmith5.png" );
   preReadCityTexture( CST_BLACKSMITH_6,        "blacksmith6.png" );
   preReadCityTexture( CST_POTTERY_0,           "pottery0.png" );
   preReadCityTexture( CST_POTTERY_1,           "pottery1.png" );
   preReadCityTexture( CST_POTTERY_2,           "pottery2.png" );
   preReadCityTexture( CST_POTTERY_3,           "pottery3.png" );
   preReadCityTexture( CST_POTTERY_4,           "pottery4.png" );
   preReadCityTexture( CST_POTTERY_5,           "pottery5.png" );
   preReadCityTexture( CST_POTTERY_6,           "pottery6.png" );
   preReadCityTexture( CST_POTTERY_7,           "pottery7.png" );
   preReadCityTexture( CST_POTTERY_8,           "pottery8.png" );
   preReadCityTexture( CST_POTTERY_9,           "pottery9.png" );
   preReadCityTexture( CST_POTTERY_10,          "pottery10.png" );
   preReadCityTexture( CST_WATER,             "water.png" );
   preReadCityTexture( CST_WATER_D,           "waterd.png" );
   preReadCityTexture( CST_WATER_R,           "waterr.png" );
   preReadCityTexture( CST_WATER_U,           "wateru.png" );
   preReadCityTexture( CST_WATER_L,           "waterl.png" );
   preReadCityTexture( CST_WATER_LR,          "waterlr.png" );
   preReadCityTexture( CST_WATER_UD,          "waterud.png" );
   preReadCityTexture( CST_WATER_LD,          "waterld.png" );
   preReadCityTexture( CST_WATER_RD,          "waterrd.png" );
   preReadCityTexture( CST_WATER_LU,          "waterlu.png" );
   preReadCityTexture( CST_WATER_UR,          "waterur.png" );
   preReadCityTexture( CST_WATER_LUD,         "waterlud.png" );
   preReadCityTexture( CST_WATER_LRD,         "waterlrd.png" );
   preReadCityTexture( CST_WATER_LUR,         "waterlur.png" );
   preReadCityTexture( CST_WATER_URD,         "waterurd.png" );
   preReadCityTexture( CST_WATER_LURD,        "waterlurd.png" );
   preReadCityTexture( CST_WATERWELL,         "waterwell.png" );
   preReadCityTexture( CST_CRICKET_1,         "cricket1.png" );
   preReadCityTexture( CST_CRICKET_2,         "cricket2.png" );
   preReadCityTexture( CST_CRICKET_3,         "cricket3.png" );
   preReadCityTexture( CST_CRICKET_4,         "cricket4.png" );
   preReadCityTexture( CST_CRICKET_5,         "cricket5.png" );
   preReadCityTexture( CST_CRICKET_6,         "cricket6.png" );
   preReadCityTexture( CST_CRICKET_7,         "cricket7.png" );
   preReadCityTexture( CST_FIRESTATION_1,       "firestation1.png" );
   preReadCityTexture( CST_FIRESTATION_2,       "firestation2.png" );
   preReadCityTexture( CST_FIRESTATION_3,       "firestation3.png" );
   preReadCityTexture( CST_FIRESTATION_4,       "firestation4.png" );
   preReadCityTexture( CST_FIRESTATION_5,       "firestation5.png" );
   preReadCityTexture( CST_FIRESTATION_6,       "firestation6.png" );
   preReadCityTexture( CST_FIRESTATION_7,       "firestation7.png" );
   preReadCityTexture( CST_FIRESTATION_8,       "firestation8.png" );
   preReadCityTexture( CST_FIRESTATION_9,       "firestation9.png" );
   preReadCityTexture( CST_FIRESTATION_10,      "firestation10.png" );
   preReadCityTexture( CST_TIP_0,             "tip0.png" );
   preReadCityTexture( CST_TIP_1,             "tip1.png" );
   preReadCityTexture( CST_TIP_2,             "tip2.png" );
   preReadCityTexture( CST_TIP_3,             "tip3.png" );
   preReadCityTexture( CST_TIP_4,             "tip4.png" );
   preReadCityTexture( CST_TIP_5,             "tip5.png" );
   preReadCityTexture( CST_TIP_6,             "tip6.png" );
   preReadCityTexture( CST_TIP_7,             "tip7.png" );
   preReadCityTexture( CST_TIP_8,             "tip8.png" );
   preReadCityTexture( CST_COMMUNE_1,         "commune1.png" );
   preReadCityTexture( CST_COMMUNE_2,         "commune2.png" );
   preReadCityTexture( CST_COMMUNE_3,         "commune3.png" );
   preReadCityTexture( CST_COMMUNE_4,         "commune4.png" );
   preReadCityTexture( CST_COMMUNE_5,         "commune5.png" );
   preReadCityTexture( CST_COMMUNE_6,         "commune6.png" );
   preReadCityTexture( CST_COMMUNE_7,         "commune7.png" );
   preReadCityTexture( CST_COMMUNE_8,         "commune8.png" );
   preReadCityTexture( CST_COMMUNE_9,         "commune9.png" );
   preReadCityTexture( CST_COMMUNE_10,        "commune10.png" );
   preReadCityTexture( CST_COMMUNE_11,        "commune11.png" );
   //preReadCityTexture( CST_COMMUNE_12,        "commune12.png" );
   //preReadCityTexture( CST_COMMUNE_13,        "commune13.png" );
   //preReadCityTexture( CST_COMMUNE_14,        "commune14.png" );
   preReadCityTexture( CST_INDUSTRY_H_C,      "industryhc.png" );
   preReadCityTexture( CST_INDUSTRY_H_L1,      "industryhl1.png" );
   preReadCityTexture( CST_INDUSTRY_H_L2,      "industryhl2.png" );
   preReadCityTexture( CST_INDUSTRY_H_L3,      "industryhl3.png" );
   preReadCityTexture( CST_INDUSTRY_H_L4,      "industryhl4.png" );
   preReadCityTexture( CST_INDUSTRY_H_L5,      "industryhl5.png" );
   preReadCityTexture( CST_INDUSTRY_H_L6,      "industryhl6.png" );
   preReadCityTexture( CST_INDUSTRY_H_L7,      "industryhl7.png" );
   preReadCityTexture( CST_INDUSTRY_H_L8,      "industryhl8.png" );
   preReadCityTexture( CST_INDUSTRY_H_M1,      "industryhm1.png" );
   preReadCityTexture( CST_INDUSTRY_H_M2,      "industryhm2.png" );
   preReadCityTexture( CST_INDUSTRY_H_M3,      "industryhm3.png" );
   preReadCityTexture( CST_INDUSTRY_H_M4,      "industryhm4.png" );
   preReadCityTexture( CST_INDUSTRY_H_M5,      "industryhm5.png" );
   preReadCityTexture( CST_INDUSTRY_H_M6,      "industryhm6.png" );
   preReadCityTexture( CST_INDUSTRY_H_M7,      "industryhm7.png" );
   preReadCityTexture( CST_INDUSTRY_H_M8,      "industryhm8.png" );
   preReadCityTexture( CST_INDUSTRY_H_H1,      "industryhh1.png" );
   preReadCityTexture( CST_INDUSTRY_H_H2,      "industryhh2.png" );
   preReadCityTexture( CST_INDUSTRY_H_H3,      "industryhh3.png" );
   preReadCityTexture( CST_INDUSTRY_H_H4,      "industryhh4.png" );
   preReadCityTexture( CST_INDUSTRY_H_H5,      "industryhh5.png" );
   preReadCityTexture( CST_INDUSTRY_H_H6,      "industryhh6.png" );
   preReadCityTexture( CST_INDUSTRY_H_H7,      "industryhh7.png" );
   preReadCityTexture( CST_INDUSTRY_H_H8,      "industryhh8.png" );
   preReadCityTexture( CST_INDUSTRY_L_C,       "industrylc.png" );
   preReadCityTexture( CST_INDUSTRY_L_Q1,      "industrylq1.png" );
   preReadCityTexture( CST_INDUSTRY_L_Q2,      "industrylq2.png" );
   preReadCityTexture( CST_INDUSTRY_L_Q3,      "industrylq3.png" );
   preReadCityTexture( CST_INDUSTRY_L_Q4,      "industrylq4.png" );
   preReadCityTexture( CST_INDUSTRY_L_L1,      "industryll1.png" );
   preReadCityTexture( CST_INDUSTRY_L_L2,      "industryll2.png" );
   preReadCityTexture( CST_INDUSTRY_L_L3,      "industryll3.png" );
   preReadCityTexture( CST_INDUSTRY_L_L4,      "industryll4.png" );
   preReadCityTexture( CST_INDUSTRY_L_M1,      "industrylm1.png" );
   preReadCityTexture( CST_INDUSTRY_L_M2,      "industrylm2.png" );
   preReadCityTexture( CST_INDUSTRY_L_M3,      "industrylm3.png" );
   preReadCityTexture( CST_INDUSTRY_L_M4,      "industrylm4.png" );
   preReadCityTexture( CST_INDUSTRY_L_H1,      "industrylh1.png" );
   preReadCityTexture( CST_INDUSTRY_L_H2,      "industrylh2.png" );
   preReadCityTexture( CST_INDUSTRY_L_H3,      "industrylh3.png" );
   preReadCityTexture( CST_INDUSTRY_L_H4,      "industrylh4.png" );
   preReadCityTexture( CST_FARM_O0,            "farm0.png" );
   //preReadCityTexture( CST_FARM_O1,            "farm1.png" );
   //preReadCityTexture( CST_FARM_O2,            "farm2.png" );
   preReadCityTexture( CST_FARM_O3,            "farm3.png" );
   //preReadCityTexture( CST_FARM_O4,            "farm4.png" );
   //preReadCityTexture( CST_FARM_O5,            "farm5.png" );
   //preReadCityTexture( CST_FARM_O6,            "farm6.png" );
   preReadCityTexture( CST_FARM_O7,            "farm7.png" );
   //preReadCityTexture( CST_FARM_O8,            "farm8.png" );
   //preReadCityTexture( CST_FARM_O9,            "farm9.png" );
   //preReadCityTexture( CST_FARM_O10,           "farm10.png" );
   preReadCityTexture( CST_FARM_O11,           "farm11.png" );
   //preReadCityTexture( CST_FARM_O12,           "farm12.png" );
   //preReadCityTexture( CST_FARM_O13,           "farm13.png" );
   //preReadCityTexture( CST_FARM_O14,           "farm14.png" );
   preReadCityTexture( CST_FARM_O15,           "farm15.png" );
   //preReadCityTexture( CST_FARM_O16,           "farm16.png" );
   preReadCityTexture( CST_TRACK_BRIDGE_LR,    "Trackbridge2.png" );
   preReadCityTexture( CST_TRACK_BRIDGE_UD,    "Trackbridge1.png" );
   preReadCityTexture( CST_TRACK_BRIDGE_LRP,   "Trackbridge_pg1.png" );
   preReadCityTexture( CST_TRACK_BRIDGE_UDP,   "Trackbridge_pg2.png" );
   preReadCityTexture( CST_TRACK_BRIDGE_ILR,   "Trackbridge_entrance_270.png" );
   preReadCityTexture( CST_TRACK_BRIDGE_OLR,   "Trackbridge_entrance_90.png" );
   preReadCityTexture( CST_TRACK_BRIDGE_IUD,   "Trackbridge_entrance_00.png" );
   preReadCityTexture( CST_TRACK_BRIDGE_OUD,   "Trackbridge_entrance_180.png" );
   preReadCityTexture( CST_ROAD_BRIDGE_LR,     "Roadbridge1.png" );
   preReadCityTexture( CST_ROAD_BRIDGE_UD,     "Roadbridge2.png" );
   preReadCityTexture( CST_ROAD_BRIDGE_LRP,    "Roadbridge1s.png" );
   preReadCityTexture( CST_ROAD_BRIDGE_UDP,    "Roadbridge2s.png" );
   preReadCityTexture( CST_ROAD_BRIDGE_LRPG,   "Roadbridge_pg1.png" );
   preReadCityTexture( CST_ROAD_BRIDGE_UDPG,   "Roadbridge_pg2.png" );
   preReadCityTexture( CST_ROAD_BRIDGE_I1LR,   "Roadbridge_entrance1_270.png" );
   preReadCityTexture( CST_ROAD_BRIDGE_O1LR,   "Roadbridge_entrance1_90.png" );
   preReadCityTexture( CST_ROAD_BRIDGE_I1UD,   "Roadbridge_entrance1_00.png" );
   preReadCityTexture( CST_ROAD_BRIDGE_O1UD,   "Roadbridge_entrance1_180.png" );
   preReadCityTexture( CST_ROAD_BRIDGE_I2LR,   "Roadbridge_entrance2_270.png" );
   preReadCityTexture( CST_ROAD_BRIDGE_O2LR,   "Roadbridge_entrance2_90.png" );
   preReadCityTexture( CST_ROAD_BRIDGE_I2UD,   "Roadbridge_entrance2_00.png" );
   preReadCityTexture( CST_ROAD_BRIDGE_O2UD,   "Roadbridge_entrance2_180.png" );
   preReadCityTexture( CST_RAIL_BRIDGE_LR,     "Railbridge1.png" );
   preReadCityTexture( CST_RAIL_BRIDGE_UD,     "Railbridge2.png" );
   preReadCityTexture( CST_RAIL_BRIDGE_LRPG,   "Railbridge_pg1.png" );
   preReadCityTexture( CST_RAIL_BRIDGE_UDPG,   "Railbridge_pg2.png" );
   preReadCityTexture( CST_RAIL_BRIDGE_I1LR,   "Railbridge_entrance1_270.png" );
   preReadCityTexture( CST_RAIL_BRIDGE_O1LR,   "Railbridge_entrance1_90.png" );
   preReadCityTexture( CST_RAIL_BRIDGE_I1UD,   "Railbridge_entrance1_00.png" );
   preReadCityTexture( CST_RAIL_BRIDGE_O1UD,   "Railbridge_entrance1_180.png" );
   preReadCityTexture( CST_RAIL_BRIDGE_I2LR,   "Railbridge_entrance2_270.png" );
   preReadCityTexture( CST_RAIL_BRIDGE_O2LR,   "Railbridge_entrance2_90.png" );
   preReadCityTexture( CST_RAIL_BRIDGE_I2UD,   "Railbridge_entrance2_00.png" );
   preReadCityTexture( CST_RAIL_BRIDGE_O2UD,   "Railbridge_entrance2_180.png" );
   
   // End of generated Code.
}

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
            if( !roadDragging && leftButtonDown && ( cursorSize == 1 ) ) {
                roadDragging = true;
                startRoad = tile;
            }
            if( roadDragging && ( cursorSize != 1 ) ){
                roadDragging = false;
            }
            // bulldoze at once while still dragging
            if( roadDragging && (selected_module_type == CST_GREEN) ){ 
                if( tile != startRoad ){
                    editMap( startRoad, SDL_BUTTON_LEFT);
                    startRoad = tile;
                }
            }
 
            if(tileUnderMouse != tile) {
                tileUnderMouse = tile;
                setDirty();
            }
            break;
        }
        case Event::MOUSEBUTTONDOWN: {
            if(!event.inside) {
                break;
            }
            if( event.mousebutton == SDL_BUTTON_RIGHT ) {
                dragging = false;
                rightButtonDown = true;
                break;       
            }
            if( event.mousebutton == SDL_BUTTON_LEFT ) {
                roadDragging = false;
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
                if ( roadDragging && event.inside ) {
                    MapPoint endRoad = getTile( event.mousepos );
                    roadDragging = false;
                    leftButtonDown = false;
                    if( cursorSize != 1 ){//roadDragging was aborted with Escape
                        break;
                    }
                    MapPoint currentTile = startRoad;
                    //build last tile first to play the sound
                    if( !blockingDialogIsOpen )
                        editMap(endRoad, SDL_BUTTON_LEFT);
                    //turn off effects for the rest of the tiles
                    bool fx = getConfig()->soundEnabled;
                    getConfig()->soundEnabled = false;
                    //use same method to find all Tiles as in void GameView::draw()
                    int stepx = ( startRoad.x > endRoad.x ) ? -1 : 1;
                    int stepy = ( startRoad.y > endRoad.y ) ? -1 : 1;
                    while( currentTile.x != endRoad.x ) {
                        if( !blockingDialogIsOpen )
                            editMap(currentTile, SDL_BUTTON_LEFT);
                        currentTile.x += stepx;
                    }
                    while( currentTile.y != endRoad.y ) {
                        if( !blockingDialogIsOpen )
                            editMap(currentTile, SDL_BUTTON_LEFT);
                        currentTile.y += stepy;
                    }
                    getConfig()->soundEnabled = fx;
                    break;
                } 
                roadDragging = false;
                leftButtonDown = false;
            }
            if(!event.inside) {
                break;
            }
            
            if( event.mousebutton == SDL_BUTTON_LEFT ){              //left
                if( !blockingDialogIsOpen )
                    editMap( getTile( event.mousepos ), SDL_BUTTON_LEFT); //edit tile
            }
            else if( event.mousebutton == SDL_BUTTON_RIGHT ){  //middle      
                recenter(event.mousepos);                      //adjust view
            }
            else if( event.mousebutton == SDL_BUTTON_WHEELUP ){ //up 
                zoomIn();                                       //zoom in
            }
            else if( event.mousebutton == SDL_BUTTON_WHEELDOWN ){ //down
                zoomOut();                                        //zoom out
            }
            break;
        case Event::KEYDOWN:
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
            break;
        case Event::KEYUP:
            //TEst
            if( event.keysym.sym == SDLK_x ){
                writeOrigin();
                readOrigin();
                break;
            }
          
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
                mapOverlay++;
                if( mapOverlay > overlayMAX ) {
                    mapOverlay = overlayNone;
                }
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
            if( event.keysym.sym == SDLK_KP_ENTER ){
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
                show(MapPoint(WORLD_SIDE_LEN / 2, WORLD_SIDE_LEN / 2));
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
    //TODO: do this only when View changed
    //Tell Minimap about new Corners
    getMiniMap()->setGameViewCorners( getTile(Vector2(0, 0)),
            getTile(Vector2(getWidth(), 0)), 
            getTile(Vector2(getWidth(), getHeight())),
            getTile(Vector2(0, getHeight()) ) );  

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
 * in oldgui you can edit (1,1) to (98,98) with WORLD_SIDE_LEN 100
 * i.e. there is a hidden border of green tiles arround the city
 */
bool GameView::inCity( MapPoint tile ){
    if( tile.x > gameAreaMax || tile.y > gameAreaMax || tile.x < gameAreaMin || tile.y < gameAreaMin ) {
        return false;
    }
    return true;
}

/*
 * Draw MiniMapOverlay for tile.
 */
void GameView::drawOverlay(Painter& painter, MapPoint tile){
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
MapPoint GameView::realTile( MapPoint tile ){
    MapPoint real = tile;
    if( ! inCity( tile ) )
        return real;
    if ( MP_TYPE( tile.x, tile.y ) ==  CST_USED ) 
    {
        real.x = MP_INFO(tile.x, tile.y).int_1;
        real.y = MP_INFO(tile.x, tile.y).int_2;    
    }
    return real;
}

void GameView::drawTile(Painter& painter, MapPoint tile)
{
    Rect2D tilerect( 0, 0, tileWidth, tileHeight );
    Vector2 tileOnScreenPoint = getScreenPoint( tile );

    //is Tile in City? If not draw Blank
    if( ! inCity( tile ) )
    {
        tileOnScreenPoint.x -= (blankTexture->getWidth() / 2)  * zoom;
        tileOnScreenPoint.y -= blankTexture->getHeight()  * zoom; 
        tilerect.move( tileOnScreenPoint );    
        tilerect.setSize(blankTexture->getWidth() * zoom,
                blankTexture->getHeight() * zoom);
        if(zoom == 1.0) 
        {
            painter.drawTexture( blankTexture, tilerect.p1 );
        }
        else
        {
            painter.drawStretchTexture( blankTexture, tilerect );
        }
        return;
    }

    Texture* texture;
    int size; 
    int upperLeftX = tile.x;
    int upperLeftY = tile.y;    

    if ( MP_TYPE( tile.x, tile.y ) ==  CST_USED ) 
    {
        upperLeftX = MP_INFO(tile.x, tile.y).int_1;
        upperLeftY = MP_INFO(tile.x, tile.y).int_2;    
    }
    size = MP_SIZE( upperLeftX, upperLeftY );

    //is Tile the lower left corner of the Building? 
    //dont't draw if not.
    if ( ( tile.x != upperLeftX ) || ( tile.y - size +1 != upperLeftY ) )
    {
        return;
    }
    //adjust OnScreenPoint of big Tiles
    if( size > 1 ) { 
        MapPoint lowerRightTile( tile.x + size - 1 , tile.y );
        tileOnScreenPoint = getScreenPoint( lowerRightTile );
    }
    
    int textureType = MP_TYPE( upperLeftX, upperLeftY );
    texture = cityTextures[ textureType ];
    // Test if we have to convert Preloaded Image to Texture
    if( !texture ) {
        SDL_mutexP( mTextures );
        if( cityImages[ textureType ] ){
            cityTextures[ textureType ] = texture_manager->create( cityImages[ textureType ] );
            cityImages[ textureType ] = 0; //Image is erased by texture_manager->create.
            texture = cityTextures[ textureType ];
        }
        SDL_mutexV( mTextures );
    }
    
    if( texture && ( !hideHigh || size == 1 ) )
    {
        /* TODO: it seems possible to put green or desert tile first,
         * so that buildings without ground will look nice
         * especially power lines :) */
        tileOnScreenPoint.x -= cityTextureX[textureType] * zoom;
        tileOnScreenPoint.y -= cityTextureY[textureType] * zoom;  
        tilerect.move( tileOnScreenPoint );    
        tilerect.setSize(texture->getWidth() * zoom, texture->getHeight() * zoom);
        if( zoom == 1.0 ) {
            painter.drawTexture(texture, tilerect.p1);
        }
        else
        {
            painter.drawStretchTexture(texture, tilerect);
        }
    }
    else 
    {
        tileOnScreenPoint.x =  tileOnScreenPoint.x - ( tileWidth*size / 2);
        tileOnScreenPoint.y -= tileHeight*size; 
        tilerect.move( tileOnScreenPoint );    
        tilerect.setSize( size * tileWidth, size * tileHeight );
        painter.setFillColor( getMiniMap()->getColorNormal( tile.x, tile.y ) );
        fillDiamond( painter, tilerect );    
    }
}

/*
 * Mark a tile with current cursor
 */
void GameView::markTile( Painter& painter, MapPoint tile )
{
    Vector2 tileOnScreenPoint = getScreenPoint(tile);
    if( cursorSize == 0 ) {
        Color alphawhite( 255, 255, 255, 128 );
        painter.setLineColor( alphawhite );
        Rect2D tilerect( 0, 0, tileWidth, tileHeight );
        tileOnScreenPoint.x = tileOnScreenPoint.x - ( tileWidth / 2);
        tileOnScreenPoint.y -= tileHeight; 
        tilerect.move( tileOnScreenPoint );    
        drawDiamond( painter, tilerect );    
    } else {
        Color alphablue( 0, 0, 255, 128 );
        Color alphared( 255, 0, 0, 128 );
        painter.setFillColor( alphablue );
        //check if building is inside the map, if not use Red Cursor
        int x = (int) tile.x;
        int y = (int) tile.y;
        MapPoint seCorner( x + cursorSize -1, y + cursorSize -1 );
        if( !inCity( seCorner ) || !inCity( tile ) ) {
            painter.setFillColor( alphared );
        } else {
            for( y = (int) tile.y; y < tile.y + cursorSize; y++ ){
                for( x = (int) tile.x; x < tile.x + cursorSize; x++ ){
                    if( !GROUP_IS_BARE(MP_GROUP( x, y ))) {
                        if( !((MP_GROUP( x, y ) == GROUP_WATER) && ( // bridge on water is OK
                           (selected_module_type == CST_TRACK_LR ) ||
                           (selected_module_type == CST_ROAD_LR ) || 
                           (selected_module_type == CST_RAIL_LR ) ))) {
                            painter.setFillColor( alphared );
                            y += cursorSize;
                            break;
                        }
                    }
                }
            }
        }
        //check if building is allowed here, if not use Red Cursor
        // These tests are in engine.cpp with place_item.
        if ( !is_allowed_here(tile.x, tile.y, selected_module_type, 0) )
            painter.setFillColor( alphared );

        Rect2D tilerect( 0, 0, tileWidth * cursorSize, tileHeight * cursorSize );
        tileOnScreenPoint.x = tileOnScreenPoint.x - (tileWidth * cursorSize / 2);
        tileOnScreenPoint.y -= tileHeight; 
        tilerect.move( tileOnScreenPoint );    
        fillDiamond( painter, tilerect );    

        // Draw range for selected_module_type
        int range = 0;
        int reduceNW = 0; // substation and market reduce the range to nort west by one. 
        switch ( selected_module_type ){
            //case CST_RESIDENCE_LL: break; 
            //case CST_RESIDENCE_ML: break;
            //case CST_RESIDENCE_HL: break;
            //case CST_RESIDENCE_LH: break;
            //case CST_RESIDENCE_MH: break;
            //case CST_RESIDENCE_HH: break;
            //case CST_FARM_O0: break;
            //case CST_MILL_0: break;
            case CST_HEALTH:        range = HEALTH_CENTRE_RANGE; break;
            case CST_CRICKET_1:     range = CRICKET_RANGE; break;
            case CST_FIRESTATION_1: range = FIRESTATION_RANGE; break;
            //case CST_SCHOOL: break;
            //case CST_UNIVERSITY: break;
            //case CST_TRACK_LR: break;
            //case CST_ROAD_LR: break;
            //case CST_RAIL_LR: break;
            //case CST_EX_PORT: break;
            //case CST_ROCKET_1: break;
            //case CST_POWERL_H_L: break;
            //case CST_POWERS_COAL_EMPTY: break;
            //case CST_POWERS_SOLAR: break;
            case CST_SUBSTATION_R:   range = SUBSTATION_RANGE; reduceNW = 1; break;
            case CST_WINDMILL_1_R:   range = SUBSTATION_RANGE; reduceNW = 1; break; //Windmills are handled like substations
            //case CST_COMMUNE_1: break;
            case CST_COALMINE_EMPTY: range = COAL_RESERVE_SEARCH_LEN; break;
            //case CST_OREMINE_1: break;
            //case CST_TIP_0: break;
            //case CST_RECYCLE: break;
            //case CST_INDUSTRY_L_C: break;
            //case CST_INDUSTRY_H_C: break;
            case CST_MARKET_EMPTY:  range = MARKET_RANGE; reduceNW = 1; break;
            //case CST_POTTERY_0: break;
            //case CST_BLACKSMITH_0: break;
            //case CST_MONUMENT_0: break;
            //case CST_PARKLAND_PLANE: break;
            //case CST_WATER: break;
            case CST_WATERWELL: range = WATERWELL_RANGE; break;
        }
       	
        if (range > 0 )
        {
        	painter.setFillColor( Color( 0, 0, 128, 64 ) );
        	Rect2D rangerect( 0,0,
        	                  tileWidth  * ( 2 * range - reduceNW ) ,
        	                  tileHeight * ( 2 * range - reduceNW ) );
        	Vector2 screenPoint = getScreenPoint(tile);
        	screenPoint.x -= tileWidth  * ( range - 0.5 * reduceNW );
        	screenPoint.y -= tileHeight * ( range + 1 - reduceNW );
        	rangerect.move( screenPoint );
        	fillDiamond( painter, rangerect );         	
        }           
    }
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
    if( centerTile.x < gameAreaMin ) {
        centerTile.x = gameAreaMin;
        outside = true;
    }
    if( centerTile.x > gameAreaMax ) {
        centerTile.x = gameAreaMax;
        outside = true;
    }
    if( centerTile.y < gameAreaMin ) {
        centerTile.y = gameAreaMin;
        outside = true;
    }
    if( centerTile.y > gameAreaMax ) {
        centerTile.y = gameAreaMax;
        outside = true;
    }
    if( outside ){
        mouseScrollState = 0;   //Avoid clipping in pause mode
        keyScrollState = 0;
        show( centerTile );
        return;
    }
    //The Corners of The Screen
    //TODO: change here to only draw dirtyRect
    //      dirtyRectangle is the current Clippingarea (if set)
    //      so we should get clippingArea (as soon this is implemented) 
    //      and adjust these Vectors:
    Vector2 upperLeft( 0, 0);
    Vector2 upperRight( getWidth(), 0 );
    Vector2 lowerLeft( 0, getHeight() );
    
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

    if( mapOverlay != overlayOnly ){
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
    if( mapOverlay != overlayNone ){
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
    //Mark Tile under Mouse 
    if( mouseInGameView  && !blockingDialogIsOpen ) {
        MapPoint lastRazed( -1,-1 );
        int tiles = 0;
        if( roadDragging && ( cursorSize == 1 ) ){
            //use same method to find all Tiles as in GameView::event(const Event& event)
            int stepx = ( startRoad.x > tileUnderMouse.x ) ? -1 : 1;
            int stepy = ( startRoad.y > tileUnderMouse.y ) ? -1 : 1;
            currentTile = startRoad;
            while( currentTile.x != tileUnderMouse.x ) {
                markTile( painter, currentTile );
                //we are speaking of tools, so CST_GREEN == bulldozer
                if( (selected_module_type == CST_GREEN) && (realTile( currentTile ) != lastRazed) ){ 
                    	cost += bulldozeCost( currentTile );
                    	lastRazed = realTile( currentTile );
                } else {
                    cost += buildCost( currentTile );
                }
                tiles++;
                currentTile.x += stepx;
            }
            while( currentTile.y != tileUnderMouse.y ) {
                markTile( painter, currentTile );
                if( (selected_module_type == CST_GREEN ) && realTile( currentTile ) != lastRazed ){ 
                    	cost += bulldozeCost( currentTile );
                    	lastRazed = realTile( currentTile );
                } else {
                    cost += buildCost( currentTile );
                }
                tiles++;
                currentTile.y += stepy;
            }
        } 
        markTile( painter, tileUnderMouse );
        tiles++;
        if( (selected_module_type == CST_GREEN ) && realTile( currentTile ) != lastRazed ) { 
            	  cost += bulldozeCost( tileUnderMouse );
        } else {
            cost += buildCost( tileUnderMouse );
        } 
        std::stringstream prize;
        if( selected_module_type == CST_GREEN ){
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
        } else if( selected_module_type == CST_TRACK_LR || selected_module_type == CST_ROAD_LR
                || selected_module_type == CST_RAIL_LR )
        {
            int group = main_types[ selected_module_type ].group;
            std::string buildingName = main_groups[ group ].name;
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
    
    if( selected_module_type == CST_NONE ) //query
    {   
        infotextstream << _("Query Tool: Show information about selected building."); 
    } 
    else if( selected_module_type == CST_GREEN ) //bulldoze
    {
        infotextstream << _("Bulldozer: remove building -price varies-"); 
    }
    else
    {
        int group = main_types[ selected_module_type ].group;
        std::string buildingName = main_groups[ group ].name;
        infotextstream << dictionaryManager->get_dictionary().translate( buildingName ); 
        infotextstream << _(": Cost to build ") << selected_module_cost <<_("$");
        infotextstream << _(", to bulldoze ") << main_groups[ group ].bul_cost <<_("$.");
        if( number > 1 ){
            infotextstream << _(" To build ") << number << _(" of them ");
            infotextstream << _("will cost about ") << number*selected_module_cost << _("$.");    
        }
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
    int group;
    int prize = 0;
    if (MP_TYPE( tile.x, tile.y) == CST_USED)
        group = MP_GROUP( MP_INFO(tile.x,tile.y).int_1,
                          MP_INFO(tile.x,tile.y).int_2 );
    else
        group = MP_GROUP( tile.x,tile.y );
    prize = main_groups[group].bul_cost;
    return prize;
}

int GameView::buildCost( MapPoint tile ){
    if( selected_module_type == CST_NONE ){
    	return 0;
    }    
    if (MP_TYPE( tile.x, tile.y ) == CST_USED)
        return 0;
    if (( selected_module_type == CST_TRACK_LR || selected_module_type == CST_ROAD_LR ||
        selected_module_type == CST_RAIL_LR) &&
        // Transport on water need a bridge
        (MP_GROUP( tile.x, tile.y) == GROUP_WATER ||
        // upgrade bridge
        ((selected_module_type == CST_ROAD_LR && (MP_GROUP( tile.x, tile.y) == GROUP_TRACK_BRIDGE)) ||
        (selected_module_type == CST_RAIL_LR && (MP_GROUP( tile.x, tile.y) == GROUP_TRACK_BRIDGE ||
        MP_GROUP( tile.x, tile.y) == GROUP_ROAD_BRIDGE))) ) )
    {
        switch( selected_module_type ) {
            case CST_TRACK_LR:
                return get_group_cost( GROUP_TRACK_BRIDGE );
            case CST_ROAD_LR:
                return get_group_cost( GROUP_ROAD_BRIDGE );
            case CST_RAIL_LR:
                return get_group_cost( GROUP_RAIL_BRIDGE );
        }
    // Not updgrade a transport
    } else if ( !GROUP_IS_BARE(MP_GROUP( tile.x, tile.y )) && (selected_module_type == CST_TRACK_LR
            || (selected_module_type == CST_ROAD_LR && (MP_GROUP( tile.x, tile.y) == GROUP_ROAD ||
                MP_GROUP( tile.x, tile.y) == GROUP_RAIL || MP_GROUP( tile.x, tile.y) == GROUP_RAIL_BRIDGE))
            || (selected_module_type == CST_RAIL_LR &&
                (MP_GROUP( tile.x, tile.y) == GROUP_RAIL || MP_GROUP( tile.x, tile.y) == GROUP_RAIL_BRIDGE))
            || (selected_module_type == CST_WATER && MP_GROUP( tile.x, tile.y) == GROUP_WATER )) ){
        return 0;
    }
   
    return get_group_cost( main_types[ selected_module_type ].group );
}

//Register as Component
IMPLEMENT_COMPONENT_FACTORY(GameView);
