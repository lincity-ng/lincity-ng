/**
 *  GameView Component for Lincity-NG.
 *   
 *  February 2005, Wolfgang Becker <uafr@gmx.de>
 *
 *  20050204
 *  +Numblock Scroll
 *  +fixed bug in drawing Big Tiles  
 *  
 *  +Zoom with Numblock + - and enter (Graphics do not get resized yet!)
 *
 *  20050205
 *  +get Offset-Info from images/tiles/images.xml
 *
 *  20050208
 *  +Zoom now working
 *
 *  20050211
 *  +show Tile under Mouse
 *
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

#include "lincity/lin-city.h"
#include "lincity/lctypes.h"
#include "lincity/engglobs.h"

#include "Mps.hpp"
#include "MapEdit.hpp"

#include <SDL_keysym.h>
#include <math.h>

GameView* gameViewPtr;

GameView* getGameView()
{
    return gameViewPtr;
}
    

GameView::GameView(Component* parent, XmlReader& reader)
    : Component(parent)
{
    gameViewPtr = this;
    //Read from config
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();
        
        //check if Attribute handled by parent
        if(parseAttribute(attribute, value)) {
            continue;
        } else {
            std::cerr << "Skipping unknown attribute '" << attribute << "'.\n";
        }
    }
    // no more elements to parse

    //Load Textures
    loadTextures();
   
    //GameView is resizable
    setFlags(FLAG_RESIZABLE);

    //start in the centre of the city
    //because on startup the size of this Control is 0
    //we use 800 and 600 instead of getWidth() and getHeight())
    //so we can not use zoom( defaultZoom ) likewise
    zoom = defaultZoom;
    tileWidth = defaultTileWidth * zoom / defaultZoom;
    tileHeight = defaultTileHeight * zoom / defaultZoom; 
    virtualScreenWidth = tileWidth * WORLD_SIDE_LEN;
    virtualScreenHeight = tileHeight * WORLD_SIDE_LEN;
    viewport.x = floor ( ( virtualScreenWidth - 800 ) / 2 );
    viewport.y = floor ( ( virtualScreenHeight- 600 ) / 2 );

    mouseInGameView = false;
    tileUnderMouse.x = 0;
    tileUnderMouse.y = 0;
}
    
GameView::~GameView()
{
    for(int i = 0; i < NUM_OF_TYPES; ++i)
        delete cityTextures[i];
    delete blankTexture;
}

/*
 * Adjust the Zoomlevel. Argument is per mille.
 */
void GameView::setZoom(const int newzoom){
    //find Tile in Center of Screen
    Vector2 center( getWidth() / 2, getHeight() / 2 );
    Vector2 centerTile  = getTile( center ); 
    
    zoom = newzoom;
    if ( zoom < 125 ) zoom = 125;
    if ( zoom > 4000 ) zoom = 4000;
    
    std::cout << "Zoom:" << zoom / 10.0 << "%\n";
    tileWidth = defaultTileWidth * zoom / defaultZoom;
    tileHeight = defaultTileHeight * zoom / defaultZoom; 
    //a virtual screen containing the whole city
    virtualScreenWidth = tileWidth * WORLD_SIDE_LEN;
    virtualScreenHeight = tileHeight * WORLD_SIDE_LEN;

    //Show the Center
    show( centerTile );
}

/* set Zoomlevel to defaultZoom (100%) */
void GameView::resetZoom(){
    setZoom( defaultZoom );
}

/* increase Zoomlevel */
void GameView::zoomIn(){
    setZoom( zoom * 2 );
}

/** decrease Zoomlevel */
void GameView::zoomOut(){
    setZoom( zoom / 2 );
}

/**
 *  Show City Tile(x/y) by centering the screen 
 */
void GameView::show( const int x, const int y )
{    
    Vector2 center;
    std::cout << "GameView::Show(" << x << "/" << y << ")\n";
    center.x = virtualScreenWidth / 2 + ( x - y ) * ( tileWidth / 2 );
    center.y = ( x + y ) * ( tileHeight / 2 ) + ( tileHeight / 2 ); 
    
    viewport.x = center.x - ( getWidth() / 2 );
    viewport.y = center.y - ( getHeight() / 2 );
    //request redraw
}
void GameView::show( const Vector2 pos )
{
    show( (int) pos.x, (int) pos.y );
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
        std::cerr << nfilename << " missing: " << err.what() << "\n";
        return 0;
    }

    return currentTexture;
}

/**
 * load a City Texture and fill in X and Y Data.
 *
 * images/tiles/images.xml contains the x-Coordinate of the 
 * middle of the Building in Case the Image is asymetric,
 * eg. a high tower with a long shadow to the right 
 *
 */
void GameView::readCityTexture( int textureType, const std::string& filename )
{
    XmlReader reader( "images/tiles/images.xml" );
    int xmlX = -1;
    int xmlY = -1;
    bool hit = false;
    
    cityTextures[ textureType ] = readTexture( filename );
    if( cityTextures[ textureType ] ) 
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
                                std::cerr << "Error parsing integer value '" << value << "' in x attribute.\n";
                                xmlX = -1;
                            }
                        }
                       else if(strcmp(name, "y") == 0 ) 
                        {
                            if(sscanf(value, "%i", &xmlY) != 1) 
                            {
                                std::cerr << "Error parsing integer value '" << value << "' in y attribute.\n";
                                xmlY = -1;
                            }
                        }
                    }//while(iter.next()) 
                    if( hit )
                    {
                        break;
                    }
                }
            }
        }//while( reader.read() ) 
       
        if( hit && ( xmlX >= 0 ) )
        { 
            cityTextureX[ textureType ] = xmlX;
        }
        else
        {
            cityTextureX[ textureType ] = int( ( cityTextures[ textureType ]->getWidth() / 2 ) );
        }
        cityTextureY[ textureType ] = int( cityTextures[ textureType ]->getHeight() ); 
    }
}
    
/**
 *  Load all required Textures
 */
const void GameView::loadTextures()
{
    memset(&cityTextures, 0, sizeof(cityTextures));
    
    blankTexture = readTexture( "blank.png" );
    
   //We need Textures for all Types from lincity/lctypes.h 
   //Code Generation:
   /*
       grep -e LCT src/lincity/lctypes.h | sed  \
           -e 's/#define LC/   readCityTexture( CS/' \
           -e 's/_G /, /'       \
           -e 's/_G\t/, /'     \
           -e 's/"/.png" );/2'                 
   */
   readCityTexture( CST_GREEN, 	"green.png" );
   readCityTexture( CST_POWERL_H_L, "powerlhl.png" );
   readCityTexture( CST_POWERL_V_L,  	"powerlvl.png" );
   readCityTexture( CST_POWERL_LD_L, 	"powerlldl.png" );
   readCityTexture( CST_POWERL_RD_L, 	"powerlrdl.png" );
   readCityTexture( CST_POWERL_LU_L,  	"powerllul.png" );
   readCityTexture( CST_POWERL_RU_L,  	"powerlrul.png" );
   readCityTexture( CST_POWERL_LDU_L, "powerlldul.png" );
   readCityTexture( CST_POWERL_LDR_L, "powerlldrl.png" );
   readCityTexture( CST_POWERL_LUR_L, "powerllurl.png" );
   readCityTexture( CST_POWERL_UDR_L, "powerludrl.png" );
   readCityTexture( CST_POWERL_LUDR_L, "powerlludrl.png" );
   readCityTexture( CST_POWERL_H_D,        "powerlhd.png" );
   readCityTexture( CST_POWERL_V_D,        "powerlvd.png" );
   readCityTexture( CST_POWERL_LD_D,       "powerlldd.png" );
   readCityTexture( CST_POWERL_RD_D,       "powerlrdd.png" );
   readCityTexture( CST_POWERL_LU_D,       "powerllud.png" );
   readCityTexture( CST_POWERL_RU_D,       "powerlrud.png" );
   readCityTexture( CST_POWERL_LDU_D,      "powerlldud.png" );
   readCityTexture( CST_POWERL_LDR_D,      "powerlldrd.png" );
   readCityTexture( CST_POWERL_LUR_D,      "powerllurd.png" );
   readCityTexture( CST_POWERL_UDR_D,      "powerludrd.png" );
   readCityTexture( CST_POWERL_LUDR_D,     "powerlludrd.png" );
   readCityTexture( CST_SHANTY,            "shanty.png" );
   readCityTexture( CST_POWERS_SOLAR, "powerssolar.png" );
   readCityTexture( CST_POWERS_COAL_EMPTY, "powerscoal-empty.png" );
   readCityTexture( CST_POWERS_COAL_LOW,   "powerscoal-low.png" );
   readCityTexture( CST_POWERS_COAL_MED,   "powerscoal-med.png" );
   readCityTexture( CST_POWERS_COAL_FULL,  "powerscoal-full.png" );
   readCityTexture( CST_BURNT, 	"burnt_land.png" );
   readCityTexture( CST_SUBSTATION_R, "substation-R.png" );
   readCityTexture( CST_SUBSTATION_G,      "substation-G.png" );
   readCityTexture( CST_SUBSTATION_RG,     "substation-RG.png" );
   readCityTexture( CST_UNIVERSITY, "university.png" );
   readCityTexture( CST_RESIDENCE_LL, "reslowlow.png" );
   readCityTexture( CST_RESIDENCE_ML, "resmedlow.png" );
   readCityTexture( CST_RESIDENCE_HL, "reshilow.png" );
   readCityTexture( CST_RESIDENCE_LH,      "reslowhi.png" );
   readCityTexture( CST_RESIDENCE_MH,      "resmedhi.png" );
   readCityTexture( CST_RESIDENCE_HH,      "reshihi.png" );
   readCityTexture( CST_MARKET_EMPTY, "market-empty.png" );
   readCityTexture( CST_MARKET_LOW,        "market-low.png" );
   readCityTexture( CST_MARKET_MED,        "market-med.png" );
   readCityTexture( CST_MARKET_FULL,       "market-full.png" );
   readCityTexture( CST_RECYCLE, 	"recycle-centre.png" );
   readCityTexture( CST_TRACK_LR, 	"tracklr.png" );
   readCityTexture( CST_TRACK_LU,          "tracklu.png" );
   readCityTexture( CST_TRACK_LD,          "trackld.png" );
   readCityTexture( CST_TRACK_UD,          "trackud.png" );
   readCityTexture( CST_TRACK_UR,          "trackur.png" );
   readCityTexture( CST_TRACK_DR,          "trackdr.png" );
   readCityTexture( CST_TRACK_LUR,         "tracklur.png" );
   readCityTexture( CST_TRACK_LDR,         "trackldr.png" );
   readCityTexture( CST_TRACK_LUD,         "tracklud.png" );
   readCityTexture( CST_TRACK_UDR,         "trackudr.png" );
   readCityTexture( CST_TRACK_LUDR,        "trackludr.png" );
   readCityTexture( CST_PARKLAND_PLANE, "parkland-plane.png" );
   readCityTexture( CST_PARKLAND_LAKE, "parkland-lake.png" );
   readCityTexture( CST_MONUMENT_0, "monument0.png" );
   readCityTexture( CST_MONUMENT_1,        "monument1.png" );
   readCityTexture( CST_MONUMENT_2,        "monument2.png" );
   readCityTexture( CST_MONUMENT_3,        "monument3.png" );
   readCityTexture( CST_MONUMENT_4,        "monument4.png" );
   readCityTexture( CST_MONUMENT_5,        "monument5.png" );
   readCityTexture( CST_COALMINE_EMPTY, "coalmine-empty.png" );
   readCityTexture( CST_COALMINE_LOW, "coalmine-low.png" );
   readCityTexture( CST_COALMINE_MED, "coalmine-med.png" );
   readCityTexture( CST_COALMINE_FULL, "coalmine-full.png" );
   readCityTexture( CST_RAIL_LR,          "raillr.png" );
   readCityTexture( CST_RAIL_LU,          "raillu.png" );
   readCityTexture( CST_RAIL_LD,          "railld.png" );
   readCityTexture( CST_RAIL_UD,          "railud.png" );
   readCityTexture( CST_RAIL_UR,          "railur.png" );
   readCityTexture( CST_RAIL_DR,          "raildr.png" );
   readCityTexture( CST_RAIL_LUR,         "raillur.png" );
   readCityTexture( CST_RAIL_LDR,         "railldr.png" );
   readCityTexture( CST_RAIL_LUD,         "raillud.png" );
   readCityTexture( CST_RAIL_UDR,         "railudr.png" );
   readCityTexture( CST_RAIL_LUDR,        "railludr.png" );
   readCityTexture( CST_FIRE_1,           "fire1.png" );
   readCityTexture( CST_FIRE_2,           "fire2.png" );
   readCityTexture( CST_FIRE_3,           "fire3.png" );
   readCityTexture( CST_FIRE_4,           "fire4.png" );
   readCityTexture( CST_FIRE_5,           "fire5.png" );
   readCityTexture( CST_FIRE_DONE1,       "firedone1.png" );
   readCityTexture( CST_FIRE_DONE2,       "firedone2.png" );
   readCityTexture( CST_FIRE_DONE3,       "firedone3.png" );
   readCityTexture( CST_FIRE_DONE4,       "firedone4.png" );
   readCityTexture( CST_ROAD_LR,          "roadlr.png" );
   readCityTexture( CST_ROAD_LU,          "roadlu.png" );
   readCityTexture( CST_ROAD_LD,          "roadld.png" );
   readCityTexture( CST_ROAD_UD,          "roadud.png" );
   readCityTexture( CST_ROAD_UR,          "roadur.png" );
   readCityTexture( CST_ROAD_DR,          "roaddr.png" );
   readCityTexture( CST_ROAD_LUR,         "roadlur.png" );
   readCityTexture( CST_ROAD_LDR,         "roadldr.png" );
   readCityTexture( CST_ROAD_LUD,         "roadlud.png" );
   readCityTexture( CST_ROAD_UDR,         "roadudr.png" );
   readCityTexture( CST_ROAD_LUDR,        "roadludr.png" );
   readCityTexture( CST_OREMINE_5,         "oremine5.png" );
   readCityTexture( CST_OREMINE_6,         "oremine6.png" );
   readCityTexture( CST_OREMINE_7,         "oremine7.png" );
   readCityTexture( CST_OREMINE_8,         "oremine8.png" );
   readCityTexture( CST_OREMINE_1, 	"oremine1.png" );
   readCityTexture( CST_OREMINE_2, 	"oremine2.png" );
   readCityTexture( CST_OREMINE_3, 	"oremine3.png" );
   readCityTexture( CST_OREMINE_4, 	"oremine4.png" );
   readCityTexture( CST_HEALTH, 	"health.png" );
   readCityTexture( CST_SCHOOL, 	"school0.png" );
   readCityTexture( CST_EX_PORT, 	"ex_port.png" );
   readCityTexture( CST_MILL_0,            "mill0.png" );
   readCityTexture( CST_MILL_1,            "mill1.png" );
   readCityTexture( CST_MILL_2,            "mill2.png" );
   readCityTexture( CST_MILL_3,            "mill3.png" );
   readCityTexture( CST_MILL_4,            "mill4.png" );
   readCityTexture( CST_MILL_5,            "mill5.png" );
   readCityTexture( CST_MILL_6,            "mill6.png" );
   readCityTexture( CST_ROCKET_1,          "rocket1.png" );
   readCityTexture( CST_ROCKET_2, 	"rocket2.png" );
   readCityTexture( CST_ROCKET_3, 	"rocket3.png" );
   readCityTexture( CST_ROCKET_4, 	"rocket4.png" );
   readCityTexture( CST_ROCKET_5,          "rocket5.png" );
   readCityTexture( CST_ROCKET_6,          "rocket6.png" );
   readCityTexture( CST_ROCKET_7, 	"rocket7.png" );
   readCityTexture( CST_ROCKET_FLOWN, "rocketflown.png" );
   readCityTexture( CST_WINDMILL_1_G,      "windmill1g.png" );
   readCityTexture( CST_WINDMILL_2_G,      "windmill2g.png" );
   readCityTexture( CST_WINDMILL_3_G,      "windmill3g.png" );
   readCityTexture( CST_WINDMILL_1_RG,     "windmill1rg.png" );
   readCityTexture( CST_WINDMILL_2_RG,     "windmill2rg.png" );
   readCityTexture( CST_WINDMILL_3_RG,     "windmill3rg.png" );
   readCityTexture( CST_WINDMILL_1_R,      "windmill1r.png" );
   readCityTexture( CST_WINDMILL_2_R,      "windmill2r.png" );
   readCityTexture( CST_WINDMILL_3_R,      "windmill3r.png" );
   readCityTexture( CST_WINDMILL_1_W,      "windmill1w.png" );
   readCityTexture( CST_WINDMILL_2_W,      "windmill2w.png" );
   readCityTexture( CST_WINDMILL_3_W,      "windmill3w.png" );
   readCityTexture( CST_BLACKSMITH_0,        "blacksmith0.png" );
   readCityTexture( CST_BLACKSMITH_1,        "blacksmith1.png" );
   readCityTexture( CST_BLACKSMITH_2,        "blacksmith2.png" );
   readCityTexture( CST_BLACKSMITH_3,        "blacksmith3.png" );
   readCityTexture( CST_BLACKSMITH_4,        "blacksmith4.png" );
   readCityTexture( CST_BLACKSMITH_5,        "blacksmith5.png" );
   readCityTexture( CST_BLACKSMITH_6,        "blacksmith6.png" );
   readCityTexture( CST_POTTERY_0,           "pottery0.png" );
   readCityTexture( CST_POTTERY_1,           "pottery1.png" );
   readCityTexture( CST_POTTERY_2,           "pottery2.png" );
   readCityTexture( CST_POTTERY_3,           "pottery3.png" );
   readCityTexture( CST_POTTERY_4,           "pottery4.png" );
   readCityTexture( CST_POTTERY_5,           "pottery5.png" );
   readCityTexture( CST_POTTERY_6,           "pottery6.png" );
   readCityTexture( CST_POTTERY_7,           "pottery7.png" );
   readCityTexture( CST_POTTERY_8,           "pottery8.png" );
   readCityTexture( CST_POTTERY_9,           "pottery9.png" );
   readCityTexture( CST_POTTERY_10,          "pottery10.png" );
   readCityTexture( CST_WATER,             "water.png" );
   readCityTexture( CST_WATER_D,           "waterd.png" );
   readCityTexture( CST_WATER_R,           "waterr.png" );
   readCityTexture( CST_WATER_U,           "wateru.png" );
   readCityTexture( CST_WATER_L,           "waterl.png" );
   readCityTexture( CST_WATER_LR,          "waterlr.png" );
   readCityTexture( CST_WATER_UD,          "waterud.png" );
   readCityTexture( CST_WATER_LD,          "waterld.png" );
   readCityTexture( CST_WATER_RD,          "waterrd.png" );
   readCityTexture( CST_WATER_LU,          "waterlu.png" );
   readCityTexture( CST_WATER_UR,          "waterur.png" );
   readCityTexture( CST_WATER_LUD,         "waterlud.png" );
   readCityTexture( CST_WATER_LRD,         "waterlrd.png" );
   readCityTexture( CST_WATER_LUR,         "waterlur.png" );
   readCityTexture( CST_WATER_URD,         "waterurd.png" );
   readCityTexture( CST_WATER_LURD,        "waterlurd.png" );
   readCityTexture( CST_CRICKET_1,         "cricket1.png" );
   readCityTexture( CST_CRICKET_2,         "cricket2.png" );
   readCityTexture( CST_CRICKET_3,         "cricket3.png" );
   readCityTexture( CST_CRICKET_4,         "cricket4.png" );
   readCityTexture( CST_CRICKET_5,         "cricket5.png" );
   readCityTexture( CST_CRICKET_6,         "cricket6.png" );
   readCityTexture( CST_CRICKET_7,         "cricket7.png" );
   readCityTexture( CST_FIRESTATION_1,       "firestation1.png" );
   readCityTexture( CST_FIRESTATION_2,       "firestation2.png" );
   readCityTexture( CST_FIRESTATION_3,       "firestation3.png" );
   readCityTexture( CST_FIRESTATION_4,       "firestation4.png" );
   readCityTexture( CST_FIRESTATION_5,       "firestation5.png" );
   readCityTexture( CST_FIRESTATION_6,       "firestation6.png" );
   readCityTexture( CST_FIRESTATION_7,       "firestation7.png" );
   readCityTexture( CST_FIRESTATION_8,       "firestation8.png" );
   readCityTexture( CST_FIRESTATION_9,       "firestation9.png" );
   readCityTexture( CST_FIRESTATION_10,      "firestation10.png" );
   readCityTexture( CST_TIP_0,             "tip0.png" );
   readCityTexture( CST_TIP_1,             "tip1.png" );
   readCityTexture( CST_TIP_2,             "tip2.png" );
   readCityTexture( CST_TIP_3,             "tip3.png" );
   readCityTexture( CST_TIP_4,             "tip4.png" );
   readCityTexture( CST_TIP_5,             "tip5.png" );
   readCityTexture( CST_TIP_6,             "tip6.png" );
   readCityTexture( CST_TIP_7,             "tip7.png" );
   readCityTexture( CST_TIP_8,             "tip8.png" );
   readCityTexture( CST_COMMUNE_1,         "commune1.png" );
   readCityTexture( CST_COMMUNE_2,         "commune2.png" );
   readCityTexture( CST_COMMUNE_3,         "commune3.png" );
   readCityTexture( CST_COMMUNE_4,         "commune4.png" );
   readCityTexture( CST_COMMUNE_5,         "commune5.png" );
   readCityTexture( CST_COMMUNE_6,         "commune6.png" );
   readCityTexture( CST_COMMUNE_7,         "commune7.png" );
   readCityTexture( CST_COMMUNE_8,         "commune8.png" );
   readCityTexture( CST_COMMUNE_9,         "commune9.png" );
   readCityTexture( CST_COMMUNE_10,        "commune10.png" );
   readCityTexture( CST_COMMUNE_11,        "commune11.png" );
   readCityTexture( CST_COMMUNE_12,        "commune12.png" );
   readCityTexture( CST_COMMUNE_13,        "commune13.png" );
   readCityTexture( CST_COMMUNE_14,        "commune14.png" );
   readCityTexture( CST_INDUSTRY_H_C,      "industryhc.png" );
   readCityTexture( CST_INDUSTRY_H_L1,      "industryhl1.png" );
   readCityTexture( CST_INDUSTRY_H_L2,      "industryhl2.png" );
   readCityTexture( CST_INDUSTRY_H_L3,      "industryhl3.png" );
   readCityTexture( CST_INDUSTRY_H_L4,      "industryhl4.png" );
   readCityTexture( CST_INDUSTRY_H_L5,      "industryhl5.png" );
   readCityTexture( CST_INDUSTRY_H_L6,      "industryhl6.png" );
   readCityTexture( CST_INDUSTRY_H_L7,      "industryhl7.png" );
   readCityTexture( CST_INDUSTRY_H_L8,      "industryhl8.png" );
   readCityTexture( CST_INDUSTRY_H_M1,      "industryhm1.png" );
   readCityTexture( CST_INDUSTRY_H_M2,      "industryhm2.png" );
   readCityTexture( CST_INDUSTRY_H_M3,      "industryhm3.png" );
   readCityTexture( CST_INDUSTRY_H_M4,      "industryhm4.png" );
   readCityTexture( CST_INDUSTRY_H_M5,      "industryhm5.png" );
   readCityTexture( CST_INDUSTRY_H_M6,      "industryhm6.png" );
   readCityTexture( CST_INDUSTRY_H_M7,      "industryhm7.png" );
   readCityTexture( CST_INDUSTRY_H_M8,      "industryhm8.png" );
   readCityTexture( CST_INDUSTRY_H_H1,      "industryhh1.png" );
   readCityTexture( CST_INDUSTRY_H_H2,      "industryhh2.png" );
   readCityTexture( CST_INDUSTRY_H_H3,      "industryhh3.png" );
   readCityTexture( CST_INDUSTRY_H_H4,      "industryhh4.png" );
   readCityTexture( CST_INDUSTRY_H_H5,      "industryhh5.png" );
   readCityTexture( CST_INDUSTRY_H_H6,      "industryhh6.png" );
   readCityTexture( CST_INDUSTRY_H_H7,      "industryhh7.png" );
   readCityTexture( CST_INDUSTRY_H_H8,      "industryhh8.png" );
   readCityTexture( CST_INDUSTRY_L_C,       "industrylq1.png" );
   readCityTexture( CST_INDUSTRY_L_Q1,      "industrylq1.png" );
   readCityTexture( CST_INDUSTRY_L_Q2,      "industrylq2.png" );
   readCityTexture( CST_INDUSTRY_L_Q3,      "industrylq3.png" );
   readCityTexture( CST_INDUSTRY_L_Q4,      "industrylq4.png" );
   readCityTexture( CST_INDUSTRY_L_L1,      "industryll1.png" );
   readCityTexture( CST_INDUSTRY_L_L2,      "industryll2.png" );
   readCityTexture( CST_INDUSTRY_L_L3,      "industryll3.png" );
   readCityTexture( CST_INDUSTRY_L_L4,      "industryll4.png" );
   readCityTexture( CST_INDUSTRY_L_M1,      "industrylm1.png" );
   readCityTexture( CST_INDUSTRY_L_M2,      "industrylm2.png" );
   readCityTexture( CST_INDUSTRY_L_M3,      "industrylm3.png" );
   readCityTexture( CST_INDUSTRY_L_M4,      "industrylm4.png" );
   readCityTexture( CST_INDUSTRY_L_H1,      "industrylh1.png" );
   readCityTexture( CST_INDUSTRY_L_H2,      "industrylh2.png" );
   readCityTexture( CST_INDUSTRY_L_H3,      "industrylh3.png" );
   readCityTexture( CST_INDUSTRY_L_H4,      "industrylh4.png" );
   readCityTexture( CST_FARM_O0,            "farm0.png" );
   readCityTexture( CST_FARM_O1,            "farm1.png" );
   readCityTexture( CST_FARM_O2,            "farm2.png" );
   readCityTexture( CST_FARM_O3,            "farm3.png" );
   readCityTexture( CST_FARM_O4,            "farm4.png" );
   readCityTexture( CST_FARM_O5,            "farm5.png" );
   readCityTexture( CST_FARM_O6,            "farm6.png" );
   readCityTexture( CST_FARM_O7,            "farm7.png" );
   readCityTexture( CST_FARM_O8,            "farm8.png" );
   readCityTexture( CST_FARM_O9,            "farm9.png" );
   readCityTexture( CST_FARM_O10,           "farm10.png" );
   readCityTexture( CST_FARM_O11,           "farm11.png" );
   readCityTexture( CST_FARM_O12,           "farm12.png" );
   readCityTexture( CST_FARM_O13,           "farm13.png" );
   readCityTexture( CST_FARM_O14,           "farm14.png" );
   readCityTexture( CST_FARM_O15,           "farm15.png" );
   readCityTexture( CST_FARM_O16,           "farm16.png" );
   // End of generated Code.
}

/*
 * Process event
 */
void GameView::event(const Event& event)
{
    int stepx = (int) floor ( tileWidth / 2 );
    int stepy = (int) floor ( tileHeight / 2 );
    Vector2 tile;
    
    switch(event.type) {
        case Event::MOUSEMOTION:
            if(!event.inside) {
                mouseInGameView = false;
                break;
            }
            mouseInGameView = true;
            tileUnderMouse = getTile( event.mousepos );
            break;
        case Event::MOUSEBUTTONUP:
            if(!event.inside) {
                break;
            }
            
            tile=getTile( event.mousepos );
            std::cout << "Tile-pos:"<<tile.x<<","<<tile.y<<std::endl;
            if( event.mousebutton == SDL_BUTTON_LEFT ){              //left
                editMap((int) tile.x, (int) tile.y,SDL_BUTTON_LEFT); //edit tile
            }
            else if( event.mousebutton == SDL_BUTTON_RIGHT ){  //right      
                recenter(event.mousepos);                      //adjust view
            }
            else if( event.mousebutton == SDL_BUTTON_MIDDLE ){ //middle
                getMPS()->setView( (int) tile.x, (int) tile.y);//show info
            }
            else if( event.mousebutton == SDL_BUTTON_WHEELUP ){ //up 
                zoomIn();                                       //zoom in
            }
            else if( event.mousebutton == SDL_BUTTON_WHEELDOWN ){ //down
                zoomOut();                                        //zoom out
            }
            break;
        case Event::KEYUP:
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
            if( event.keysym.mod & KMOD_SHIFT ){
                stepx =  (int) 5 * tileWidth;
                stepy =  (int) 5 * tileHeight;
            } 
            if ( event.keysym.sym == SDLK_KP9 ) {
                viewport.x += stepx;
                viewport.y -= stepy;
                break;
            }
            if ( event.keysym.sym == SDLK_KP1 ) {
                viewport.x -= stepx;
                viewport.y += stepy;
                break;
            }
            if ( event.keysym.sym == SDLK_KP8 ) {
                viewport.y -= stepy;
                break;
            }
            if ( event.keysym.sym == SDLK_KP2 ) {
                viewport.y += stepy;
                break;
            }
            if ( event.keysym.sym == SDLK_KP7 ) {
                viewport.x -= stepx;
                viewport.y -= stepy;
                break;
            }
            if ( event.keysym.sym == SDLK_KP3 ) {
                viewport.x += stepx;
                viewport.y += stepy;
                break;
            }
            if ( event.keysym.sym == SDLK_KP6 ) {
                viewport.x += stepx;
                break;
            }
            if ( event.keysym.sym == SDLK_KP4 ) {
                viewport.x -= stepx;
                break;
            }
            if ( event.keysym.sym == SDLK_KP5 ) {
                show( WORLD_SIDE_LEN / 2, WORLD_SIDE_LEN / 2 );
                break;
            }
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
    //request redraw
}

/*
 * Pos is new Center of the Screen
 */
const void GameView::recenter(const Vector2& pos)
{
    Vector2 position = pos + viewport;
    viewport.x = floor( position.x - ( getWidth() / 2 ) );
    viewport.y = floor( position.y - ( getHeight() / 2 ) );
    
    //request redraw
}

/*
 * Find point on Screen, where lower right corner of tile
 * is placed.
 */
const Vector2 GameView::getScreenPoint(const Vector2& tile)
{
    Vector2 point;
    point.x = virtualScreenWidth / 2 + ( tile.x - tile.y ) * ( tileWidth / 2 );
    point.y = ( tile.x + tile.y ) * ( tileHeight / 2 ); 
    
    //we want the lower right corner
    point.y += tileHeight;
    //on Screen
    point -= viewport; 

    return point;
}

/*
 * Find Tile at point on viewport
 */
const Vector2 GameView::getTile(const Vector2& p)
{
    Vector2 tile;
    // Map Point to virtual Screen
    Vector2 point = p + viewport;
    tile.x = (point.x - virtualScreenWidth / 2 ) / tileWidth +  point.y  / tileHeight;
    tile.y =  2 * point.y  / tileHeight  - tile.x; 
    tile.x = floor( tile.x );
    tile.y = floor( tile.y );

    return tile;
}

/*
 *  Draw a Tile
 */
const void GameView::drawTile(Painter& painter, const Vector2& tile)
{
    int tx = (int) tile.x;
    int ty = (int) tile.y;
    
    Color red;
    red.parse( "red" );

    Rect2D tilerect( 0, 0, tileWidth, tileHeight );
    Vector2 tileOnScreenPoint = getScreenPoint( tile );

    //is Tile in City? If not draw Blank
    if( tx < 0 || ty < 0 || tx > WORLD_SIDE_LEN || ty > WORLD_SIDE_LEN )
    {
        tileOnScreenPoint.x -= ( ( blankTexture->getWidth() / 2 )  * zoom / defaultZoom );
        tileOnScreenPoint.y -= (blankTexture->getHeight()  * zoom / defaultZoom ); 
        tilerect.move( tileOnScreenPoint );    
        tilerect.setSize( blankTexture->getWidth()  * zoom / defaultZoom, blankTexture->getHeight() * zoom / defaultZoom );
        if( zoom == defaultZoom ) 
        {
            painter.drawTexture(blankTexture, tilerect.p1);
        }
        else
        {
            painter.drawStretchTexture(blankTexture, tilerect);
        }
        return;
    }

    Texture* texture;
    int size; 
    int upperLeftX = tx;
    int upperLeftY = ty;    

    if ( MP_TYPE( tx, ty ) ==  CST_USED ) 
    {
        upperLeftX = MP_INFO(tx,ty).int_1;
        upperLeftY = MP_INFO(tx,ty).int_2;    
    }
    size = MP_SIZE( upperLeftX, upperLeftY );

    //is Tile the lower left corner of the Building? 
    //dont't draw if not.
    if ( ( tx != upperLeftX ) || ( ty - size +1 != upperLeftY ) )
    {
        return;
    }
    //adjust OnScreenPoint of big Tiles
    if( size > 1 ) { 
        Vector2 lowerRightTile( tile.x + size - 1 , tile.y );
        tileOnScreenPoint = getScreenPoint( lowerRightTile );
    }
    
    int textureType = MP_TYPE( upperLeftX, upperLeftY );
    texture = cityTextures[ textureType ];
    
    if( texture )
    {
        tileOnScreenPoint.x -= ( cityTextureX[ textureType ] * zoom / defaultZoom );
        tileOnScreenPoint.y -= ( cityTextureY[ textureType ] * zoom / defaultZoom );  
        tilerect.move( tileOnScreenPoint );    
        tilerect.setSize( texture->getWidth() * zoom / defaultZoom, texture->getHeight() * zoom / defaultZoom );
        if( zoom == defaultZoom ) {
            painter.drawTexture(texture, tilerect.p1);
        }
        else
        {
            painter.drawStretchTexture(texture, tilerect);
        }
    }
    else 
    {
    //std::cout << "drawTile Texture Missing " << MP_TYPE( upperLeftX, upperLeftY ) << "\n";
        tileOnScreenPoint.x =  floor( tileOnScreenPoint.x - ( tileWidth / 2));
        tileOnScreenPoint.y -= tileHeight; 
        tilerect.move( tileOnScreenPoint );    
        painter.setFillColor( red );
        //painter.fillDiamond( tilerect );    
    }
}

/*
 *  Paint an isometric View of the City in the component.
 */
void GameView::draw(Painter& painter)
{
    //The Corners of The Screen
    Vector2 upperLeft( 0, 0);
    Vector2 upperRight( getWidth(), 0 );
    Vector2 lowerLeft( 0, getHeight() );
    
    //Find visible Tiles
    Vector2 upperLeftTile  = getTile( upperLeft ); 
    Vector2 upperRightTile = getTile( upperRight );
    Vector2 lowerLeftTile  = getTile( lowerLeft ); 
    
    //draw Background
    Color green;
    Rect2D background( 0, 0, getWidth(), getHeight() );
    green.parse( "green" );
    painter.setFillColor( green );
    painter.fillRectangle( background );    

    //draw Tiles
    Vector2 currentTile;
    //Draw some extra tiles depending on the maximal size of a building.
    static const int extratiles = 4;
    upperLeftTile.x -= extratiles;
    upperRightTile.y -= extratiles;
    lowerLeftTile.y +=  extratiles;

    int i, k;
    for( k = 0; k <= 2 * ( lowerLeftTile.y - upperLeftTile.y ); k++ )
    {
        for( i = 0; i <= upperRightTile.x - upperLeftTile.x; i++ )
        {
            currentTile.x = upperLeftTile.x + i + floor( k / 2 ) + k % 2;
            currentTile.y = upperLeftTile.y - i + floor( k / 2 );
            drawTile( painter, currentTile );
        }
    }
    
    //Mark Tile under Mouse 
    if( mouseInGameView )
    {
        Color alphablue( 0, 0, 255, 128 );
        painter.setFillColor( alphablue );
        Rect2D tilerect( 0, 0, tileWidth, tileHeight );
        Vector2 tileOnScreenPoint = getScreenPoint( tileUnderMouse );
        tileOnScreenPoint.x =  floor( tileOnScreenPoint.x - ( tileWidth / 2));
        tileOnScreenPoint.y -= tileHeight; 
        tilerect.move( tileOnScreenPoint );    
        //painter.fillDiamond( tilerect );    
    }
}

//Register as Component
IMPLEMENT_COMPONENT_FACTORY(GameView)
