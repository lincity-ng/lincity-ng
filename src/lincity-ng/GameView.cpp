/**
 *  GameView Component for Lincity-NG.
 *   
 *  February 2005, Wolfgang Becker <uafr@gmx.de>
 *
 *  20050204
 *  +Numblock Scroll
 *  +fixed bug in drawing Big Tiles  
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

#include <SDL_keysym.h>
#include <math.h>

GameView::GameView(Component* parent, XmlReader& reader)
    : Component(parent)
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
    viewport.x = floor ( ( virtualScreenWidth - 800 ) / 2 );
    viewport.y = floor ( ( virtualScreenHeight- 600 ) / 2 );
}

GameView::~GameView()
{
    for(int i = 0; i < NUM_OF_TYPES; ++i)
        delete cityTextures[i];
    delete blankTexture;
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
    } catch(std::runtime_error err) {
        std::cerr << nfilename << " missing: " << err.what() << "\n";
        return 0;
    }

    return currentTexture;
}

/**
 *  Load Textures
 */
const void GameView::loadTextures()
{
    memset(&cityTextures, 0, sizeof(cityTextures));
    
    blankTexture = readTexture( "blank.png" );
    
   //We need Textures for all Types from lincity/lctypes.h 
   // quick and dirty Code Generation:
   /*
       grep -e LCT src/lincity/lctypes.h | sed  \
           -e 's/#define LC/cityTextures[ CS/g' \
           -e 's/_G / ] = readTexture( /'       \
           -e 's/_G\t/ ] = readTexture( /'     \
           -e 's/"/"images\/gameview\//'       \
           -e 's/"/.png" );/2'                 
   */

    cityTextures[ CST_GREEN ] = readTexture( 	"green.png" );
    cityTextures[ CST_POWERL_H_L ] = readTexture( "powerlhl.png" );
    cityTextures[ CST_POWERL_V_L ] = readTexture(  	"powerlvl.png" );
    cityTextures[ CST_POWERL_LD_L ] = readTexture( 	"powerlldl.png" );
    cityTextures[ CST_POWERL_RD_L ] = readTexture( 	"powerlrdl.png" );
    cityTextures[ CST_POWERL_LU_L ] = readTexture(  	"powerllul.png" );
    cityTextures[ CST_POWERL_RU_L ] = readTexture(  	"powerlrul.png" );
    cityTextures[ CST_POWERL_LDU_L ] = readTexture( "powerlldul.png" );
    cityTextures[ CST_POWERL_LDR_L ] = readTexture( "powerlldrl.png" );
    cityTextures[ CST_POWERL_LUR_L ] = readTexture( "powerllurl.png" );
    cityTextures[ CST_POWERL_UDR_L ] = readTexture( "powerludrl.png" );
    cityTextures[ CST_POWERL_LUDR_L ] = readTexture( "powerlludrl.png" );
    cityTextures[ CST_POWERL_H_D ] = readTexture(        "powerlhd.png" );
    cityTextures[ CST_POWERL_V_D ] = readTexture(        "powerlvd.png" );
    cityTextures[ CST_POWERL_LD_D ] = readTexture(       "powerlldd.png" );
    cityTextures[ CST_POWERL_RD_D ] = readTexture(       "powerlrdd.png" );
    cityTextures[ CST_POWERL_LU_D ] = readTexture(       "powerllud.png" );
    cityTextures[ CST_POWERL_RU_D ] = readTexture(       "powerlrud.png" );
    cityTextures[ CST_POWERL_LDU_D ] = readTexture(      "powerlldud.png" );
    cityTextures[ CST_POWERL_LDR_D ] = readTexture(      "powerlldrd.png" );
    cityTextures[ CST_POWERL_LUR_D ] = readTexture(      "powerllurd.png" );
    cityTextures[ CST_POWERL_UDR_D ] = readTexture(      "powerludrd.png" );
    cityTextures[ CST_POWERL_LUDR_D ] = readTexture(     "powerlludrd.png" );
    cityTextures[ CST_SHANTY ] = readTexture(            "shanty.png" );
    cityTextures[ CST_POWERS_SOLAR ] = readTexture( "powerssolar.png" );
    cityTextures[ CST_POWERS_COAL_EMPTY ] = readTexture( "powerscoal-empty.png" );
    cityTextures[ CST_POWERS_COAL_LOW ] = readTexture(   "powerscoal-low.png" );
    cityTextures[ CST_POWERS_COAL_MED ] = readTexture(   "powerscoal-med.png" );
    cityTextures[ CST_POWERS_COAL_FULL ] = readTexture(  "powerscoal-full.png" );
    cityTextures[ CST_BURNT ] = readTexture( 	"burnt_land.png" );
    cityTextures[ CST_SUBSTATION_R ] = readTexture( "substation-R.png" );
    cityTextures[ CST_SUBSTATION_G ] = readTexture(      "substation-G.png" );
    cityTextures[ CST_SUBSTATION_RG ] = readTexture(     "substation-RG.png" );
    cityTextures[ CST_UNIVERSITY ] = readTexture( "university.png" );
    cityTextures[ CST_RESIDENCE_LL ] = readTexture( "reslowlow.png" );
    cityTextures[ CST_RESIDENCE_ML ] = readTexture( "resmedlow.png" );
    cityTextures[ CST_RESIDENCE_HL ] = readTexture( "reshilow.png" );
    cityTextures[ CST_RESIDENCE_LH ] = readTexture(      "reslowhi.png" );
    cityTextures[ CST_RESIDENCE_MH ] = readTexture(      "resmedhi.png" );
    cityTextures[ CST_RESIDENCE_HH ] = readTexture(      "reshihi.png" );
    cityTextures[ CST_MARKET_EMPTY ] = readTexture( "market-empty.png" );
    cityTextures[ CST_MARKET_LOW ] = readTexture(        "market-low.png" );
    cityTextures[ CST_MARKET_MED ] = readTexture(        "market-med.png" );
    cityTextures[ CST_MARKET_FULL ] = readTexture(       "market-full.png" );
    cityTextures[ CST_RECYCLE ] = readTexture( 	"recycle-centre.png" );
    cityTextures[ CST_TRACK_LR ] = readTexture( 	"tracklr.png" );
    cityTextures[ CST_TRACK_LU ] = readTexture(          "tracklu.png" );
    cityTextures[ CST_TRACK_LD ] = readTexture(          "trackld.png" );
    cityTextures[ CST_TRACK_UD ] = readTexture(          "trackud.png" );
    cityTextures[ CST_TRACK_UR ] = readTexture(          "trackur.png" );
    cityTextures[ CST_TRACK_DR ] = readTexture(          "trackdr.png" );
    cityTextures[ CST_TRACK_LUR ] = readTexture(         "tracklur.png" );
    cityTextures[ CST_TRACK_LDR ] = readTexture(         "trackldr.png" );
    cityTextures[ CST_TRACK_LUD ] = readTexture(         "tracklud.png" );
    cityTextures[ CST_TRACK_UDR ] = readTexture(         "trackudr.png" );
    cityTextures[ CST_TRACK_LUDR ] = readTexture(        "trackludr.png" );
    cityTextures[ CST_PARKLAND_PLANE ] = readTexture( "parkland-plane.png" );
    cityTextures[ CST_PARKLAND_LAKE ] = readTexture( "parkland-lake.png" );
    cityTextures[ CST_MONUMENT_0 ] = readTexture( "monument0.png" );
    cityTextures[ CST_MONUMENT_1 ] = readTexture(        "monument1.png" );
    cityTextures[ CST_MONUMENT_2 ] = readTexture(        "monument2.png" );
    cityTextures[ CST_MONUMENT_3 ] = readTexture(        "monument3.png" );
    cityTextures[ CST_MONUMENT_4 ] = readTexture(        "monument4.png" );
    cityTextures[ CST_MONUMENT_5 ] = readTexture(        "monument5.png" );
    cityTextures[ CST_COALMINE_EMPTY ] = readTexture( "coalmine-empty.png" );
    cityTextures[ CST_COALMINE_LOW ] = readTexture( "coalmine-low.png" );
    cityTextures[ CST_COALMINE_MED ] = readTexture( "coalmine-med.png" );
    cityTextures[ CST_COALMINE_FULL ] = readTexture( "coalmine-full.png" );
    cityTextures[ CST_RAIL_LR ] = readTexture(          "raillr.png" );
    cityTextures[ CST_RAIL_LU ] = readTexture(          "raillu.png" );
    cityTextures[ CST_RAIL_LD ] = readTexture(          "railld.png" );
    cityTextures[ CST_RAIL_UD ] = readTexture(          "railud.png" );
    cityTextures[ CST_RAIL_UR ] = readTexture(          "railur.png" );
    cityTextures[ CST_RAIL_DR ] = readTexture(          "raildr.png" );
    cityTextures[ CST_RAIL_LUR ] = readTexture(         "raillur.png" );
    cityTextures[ CST_RAIL_LDR ] = readTexture(         "railldr.png" );
    cityTextures[ CST_RAIL_LUD ] = readTexture(         "raillud.png" );
    cityTextures[ CST_RAIL_UDR ] = readTexture(         "railudr.png" );
    cityTextures[ CST_RAIL_LUDR ] = readTexture(        "railludr.png" );
    cityTextures[ CST_FIRE_1 ] = readTexture(           "fire1.png" );
    cityTextures[ CST_FIRE_2 ] = readTexture(           "fire2.png" );
    cityTextures[ CST_FIRE_3 ] = readTexture(           "fire3.png" );
    cityTextures[ CST_FIRE_4 ] = readTexture(           "fire4.png" );
    cityTextures[ CST_FIRE_5 ] = readTexture(           "fire5.png" );
    cityTextures[ CST_FIRE_DONE1 ] = readTexture(       "firedone1.png" );
    cityTextures[ CST_FIRE_DONE2 ] = readTexture(       "firedone2.png" );
    cityTextures[ CST_FIRE_DONE3 ] = readTexture(       "firedone3.png" );
    cityTextures[ CST_FIRE_DONE4 ] = readTexture(       "firedone4.png" );
    cityTextures[ CST_ROAD_LR ] = readTexture(          "roadlr.png" );
    cityTextures[ CST_ROAD_LU ] = readTexture(          "roadlu.png" );
    cityTextures[ CST_ROAD_LD ] = readTexture(          "roadld.png" );
    cityTextures[ CST_ROAD_UD ] = readTexture(          "roadud.png" );
    cityTextures[ CST_ROAD_UR ] = readTexture(          "roadur.png" );
    cityTextures[ CST_ROAD_DR ] = readTexture(          "roaddr.png" );
    cityTextures[ CST_ROAD_LUR ] = readTexture(         "roadlur.png" );
    cityTextures[ CST_ROAD_LDR ] = readTexture(         "roadldr.png" );
    cityTextures[ CST_ROAD_LUD ] = readTexture(         "roadlud.png" );
    cityTextures[ CST_ROAD_UDR ] = readTexture(         "roadudr.png" );
    cityTextures[ CST_ROAD_LUDR ] = readTexture(        "roadludr.png" );
    cityTextures[ CST_OREMINE_5 ] = readTexture(         "oremine5.png" );
    cityTextures[ CST_OREMINE_6 ] = readTexture(         "oremine6.png" );
    cityTextures[ CST_OREMINE_7 ] = readTexture(         "oremine7.png" );
    cityTextures[ CST_OREMINE_8 ] = readTexture(         "oremine8.png" );
    cityTextures[ CST_OREMINE_1 ] = readTexture( 	"oremine1.png" );
    cityTextures[ CST_OREMINE_2 ] = readTexture( 	"oremine2.png" );
    cityTextures[ CST_OREMINE_3 ] = readTexture( 	"oremine3.png" );
    cityTextures[ CST_OREMINE_4 ] = readTexture( 	"oremine4.png" );
    cityTextures[ CST_HEALTH ] = readTexture( 	"health.png" );
    cityTextures[ CST_SCHOOL ] = readTexture( 	"school0.png" );
    cityTextures[ CST_EX_PORT ] = readTexture( 	"ex_port.png" );
    cityTextures[ CST_MILL_0 ] = readTexture(            "mill0.png" );
    cityTextures[ CST_MILL_1 ] = readTexture(            "mill1.png" );
    cityTextures[ CST_MILL_2 ] = readTexture(            "mill2.png" );
    cityTextures[ CST_MILL_3 ] = readTexture(            "mill3.png" );
    cityTextures[ CST_MILL_4 ] = readTexture(            "mill4.png" );
    cityTextures[ CST_MILL_5 ] = readTexture(            "mill5.png" );
    cityTextures[ CST_MILL_6 ] = readTexture(            "mill6.png" );
    cityTextures[ CST_ROCKET_1 ] = readTexture(          "rocket1.png" );
    cityTextures[ CST_ROCKET_2 ] = readTexture( 	"rocket2.png" );
    cityTextures[ CST_ROCKET_3 ] = readTexture( 	"rocket3.png" );
    cityTextures[ CST_ROCKET_4 ] = readTexture( 	"rocket4.png" );
    cityTextures[ CST_ROCKET_5 ] = readTexture(          "rocket5.png" );
    cityTextures[ CST_ROCKET_6 ] = readTexture(          "rocket6.png" );
    cityTextures[ CST_ROCKET_7 ] = readTexture( 	"rocket7.png" );
    cityTextures[ CST_ROCKET_FLOWN ] = readTexture( "rocketflown.png" );
    cityTextures[ CST_WINDMILL_1_G ] = readTexture(      "windmill1g.png" );
    cityTextures[ CST_WINDMILL_2_G ] = readTexture(      "windmill2g.png" );
    cityTextures[ CST_WINDMILL_3_G ] = readTexture(      "windmill3g.png" );
    cityTextures[ CST_WINDMILL_1_RG ] = readTexture(     "windmill1rg.png" );
    cityTextures[ CST_WINDMILL_2_RG ] = readTexture(     "windmill2rg.png" );
    cityTextures[ CST_WINDMILL_3_RG ] = readTexture(     "windmill3rg.png" );
    cityTextures[ CST_WINDMILL_1_R ] = readTexture(      "windmill1r.png" );
    cityTextures[ CST_WINDMILL_2_R ] = readTexture(      "windmill2r.png" );
    cityTextures[ CST_WINDMILL_3_R ] = readTexture(      "windmill3r.png" );
    cityTextures[ CST_WINDMILL_1_W ] = readTexture(      "windmill1w.png" );
    cityTextures[ CST_WINDMILL_2_W ] = readTexture(      "windmill2w.png" );
    cityTextures[ CST_WINDMILL_3_W ] = readTexture(      "windmill3w.png" );
    cityTextures[ CST_BLACKSMITH_0 ] = readTexture(        "blacksmith0.png" );
    cityTextures[ CST_BLACKSMITH_1 ] = readTexture(        "blacksmith1.png" );
    cityTextures[ CST_BLACKSMITH_2 ] = readTexture(        "blacksmith2.png" );
    cityTextures[ CST_BLACKSMITH_3 ] = readTexture(        "blacksmith3.png" );
    cityTextures[ CST_BLACKSMITH_4 ] = readTexture(        "blacksmith4.png" );
    cityTextures[ CST_BLACKSMITH_5 ] = readTexture(        "blacksmith5.png" );
    cityTextures[ CST_BLACKSMITH_6 ] = readTexture(        "blacksmith6.png" );
    cityTextures[ CST_POTTERY_0 ] = readTexture(           "pottery0.png" );
    cityTextures[ CST_POTTERY_1 ] = readTexture(           "pottery1.png" );
    cityTextures[ CST_POTTERY_2 ] = readTexture(           "pottery2.png" );
    cityTextures[ CST_POTTERY_3 ] = readTexture(           "pottery3.png" );
    cityTextures[ CST_POTTERY_4 ] = readTexture(           "pottery4.png" );
    cityTextures[ CST_POTTERY_5 ] = readTexture(           "pottery5.png" );
    cityTextures[ CST_POTTERY_6 ] = readTexture(           "pottery6.png" );
    cityTextures[ CST_POTTERY_7 ] = readTexture(           "pottery7.png" );
    cityTextures[ CST_POTTERY_8 ] = readTexture(           "pottery8.png" );
    cityTextures[ CST_POTTERY_9 ] = readTexture(           "pottery9.png" );
    cityTextures[ CST_POTTERY_10 ] = readTexture(          "pottery10.png" );
    cityTextures[ CST_WATER ] = readTexture(             "water.png" );
    cityTextures[ CST_WATER_D ] = readTexture(           "waterd.png" );
    cityTextures[ CST_WATER_R ] = readTexture(           "waterr.png" );
    cityTextures[ CST_WATER_U ] = readTexture(           "wateru.png" );
    cityTextures[ CST_WATER_L ] = readTexture(           "waterl.png" );
    cityTextures[ CST_WATER_LR ] = readTexture(          "waterlr.png" );
    cityTextures[ CST_WATER_UD ] = readTexture(          "waterud.png" );
    cityTextures[ CST_WATER_LD ] = readTexture(          "waterld.png" );
    cityTextures[ CST_WATER_RD ] = readTexture(          "waterrd.png" );
    cityTextures[ CST_WATER_LU ] = readTexture(          "waterlu.png" );
    cityTextures[ CST_WATER_UR ] = readTexture(          "waterur.png" );
    cityTextures[ CST_WATER_LUD ] = readTexture(         "waterlud.png" );
    cityTextures[ CST_WATER_LRD ] = readTexture(         "waterlrd.png" );
    cityTextures[ CST_WATER_LUR ] = readTexture(         "waterlur.png" );
    cityTextures[ CST_WATER_URD ] = readTexture(         "waterurd.png" );
    cityTextures[ CST_WATER_LURD ] = readTexture(        "waterlurd.png" );
    cityTextures[ CST_CRICKET_1 ] = readTexture(         "cricket1.png" );
    cityTextures[ CST_CRICKET_2 ] = readTexture(         "cricket2.png" );
    cityTextures[ CST_CRICKET_3 ] = readTexture(         "cricket3.png" );
    cityTextures[ CST_CRICKET_4 ] = readTexture(         "cricket4.png" );
    cityTextures[ CST_CRICKET_5 ] = readTexture(         "cricket5.png" );
    cityTextures[ CST_CRICKET_6 ] = readTexture(         "cricket6.png" );
    cityTextures[ CST_CRICKET_7 ] = readTexture(         "cricket7.png" );
    cityTextures[ CST_FIRESTATION_1 ] = readTexture(       "firestation1.png" );
    cityTextures[ CST_FIRESTATION_2 ] = readTexture(       "firestation2.png" );
    cityTextures[ CST_FIRESTATION_3 ] = readTexture(       "firestation3.png" );
    cityTextures[ CST_FIRESTATION_4 ] = readTexture(       "firestation4.png" );
    cityTextures[ CST_FIRESTATION_5 ] = readTexture(       "firestation5.png" );
    cityTextures[ CST_FIRESTATION_6 ] = readTexture(       "firestation6.png" );
    cityTextures[ CST_FIRESTATION_7 ] = readTexture(       "firestation7.png" );
    cityTextures[ CST_FIRESTATION_8 ] = readTexture(       "firestation8.png" );
    cityTextures[ CST_FIRESTATION_9 ] = readTexture(       "firestation9.png" );
    cityTextures[ CST_FIRESTATION_10 ] = readTexture(      "firestation10.png" );
    cityTextures[ CST_TIP_0 ] = readTexture(             "tip0.png" );
    cityTextures[ CST_TIP_1 ] = readTexture(             "tip1.png" );
    cityTextures[ CST_TIP_2 ] = readTexture(             "tip2.png" );
    cityTextures[ CST_TIP_3 ] = readTexture(             "tip3.png" );
    cityTextures[ CST_TIP_4 ] = readTexture(             "tip4.png" );
    cityTextures[ CST_TIP_5 ] = readTexture(             "tip5.png" );
    cityTextures[ CST_TIP_6 ] = readTexture(             "tip6.png" );
    cityTextures[ CST_TIP_7 ] = readTexture(             "tip7.png" );
    cityTextures[ CST_TIP_8 ] = readTexture(             "tip8.png" );
    cityTextures[ CST_COMMUNE_1 ] = readTexture(         "commune1.png" );
    cityTextures[ CST_COMMUNE_2 ] = readTexture(         "commune2.png" );
    cityTextures[ CST_COMMUNE_3 ] = readTexture(         "commune3.png" );
    cityTextures[ CST_COMMUNE_4 ] = readTexture(         "commune4.png" );
    cityTextures[ CST_COMMUNE_5 ] = readTexture(         "commune5.png" );
    cityTextures[ CST_COMMUNE_6 ] = readTexture(         "commune6.png" );
    cityTextures[ CST_COMMUNE_7 ] = readTexture(         "commune7.png" );
    cityTextures[ CST_COMMUNE_8 ] = readTexture(         "commune8.png" );
    cityTextures[ CST_COMMUNE_9 ] = readTexture(         "commune9.png" );
    cityTextures[ CST_COMMUNE_10 ] = readTexture(        "commune10.png" );
    cityTextures[ CST_COMMUNE_11 ] = readTexture(        "commune11.png" );
    cityTextures[ CST_COMMUNE_12 ] = readTexture(        "commune12.png" );
    cityTextures[ CST_COMMUNE_13 ] = readTexture(        "commune13.png" );
    cityTextures[ CST_COMMUNE_14 ] = readTexture(        "commune14.png" );
    cityTextures[ CST_INDUSTRY_H_C ] = readTexture(      "industryhc.png" );
    cityTextures[ CST_INDUSTRY_H_L1 ] = readTexture(      "industryhl1.png" );
    cityTextures[ CST_INDUSTRY_H_L2 ] = readTexture(      "industryhl2.png" );
    cityTextures[ CST_INDUSTRY_H_L3 ] = readTexture(      "industryhl3.png" );
    cityTextures[ CST_INDUSTRY_H_L4 ] = readTexture(      "industryhl4.png" );
    cityTextures[ CST_INDUSTRY_H_L5 ] = readTexture(      "industryhl5.png" );
    cityTextures[ CST_INDUSTRY_H_L6 ] = readTexture(      "industryhl6.png" );
    cityTextures[ CST_INDUSTRY_H_L7 ] = readTexture(      "industryhl7.png" );
    cityTextures[ CST_INDUSTRY_H_L8 ] = readTexture(      "industryhl8.png" );
    cityTextures[ CST_INDUSTRY_H_M1 ] = readTexture(      "industryhm1.png" );
    cityTextures[ CST_INDUSTRY_H_M2 ] = readTexture(      "industryhm2.png" );
    cityTextures[ CST_INDUSTRY_H_M3 ] = readTexture(      "industryhm3.png" );
    cityTextures[ CST_INDUSTRY_H_M4 ] = readTexture(      "industryhm4.png" );
    cityTextures[ CST_INDUSTRY_H_M5 ] = readTexture(      "industryhm5.png" );
    cityTextures[ CST_INDUSTRY_H_M6 ] = readTexture(      "industryhm6.png" );
    cityTextures[ CST_INDUSTRY_H_M7 ] = readTexture(      "industryhm7.png" );
    cityTextures[ CST_INDUSTRY_H_M8 ] = readTexture(      "industryhm8.png" );
    cityTextures[ CST_INDUSTRY_H_H1 ] = readTexture(      "industryhh1.png" );
    cityTextures[ CST_INDUSTRY_H_H2 ] = readTexture(      "industryhh2.png" );
    cityTextures[ CST_INDUSTRY_H_H3 ] = readTexture(      "industryhh3.png" );
    cityTextures[ CST_INDUSTRY_H_H4 ] = readTexture(      "industryhh4.png" );
    cityTextures[ CST_INDUSTRY_H_H5 ] = readTexture(      "industryhh5.png" );
    cityTextures[ CST_INDUSTRY_H_H6 ] = readTexture(      "industryhh6.png" );
    cityTextures[ CST_INDUSTRY_H_H7 ] = readTexture(      "industryhh7.png" );
    cityTextures[ CST_INDUSTRY_H_H8 ] = readTexture(      "industryhh8.png" );
    cityTextures[ CST_INDUSTRY_L_C ] = readTexture(       "industrylq1.png" );
    cityTextures[ CST_INDUSTRY_L_Q1 ] = readTexture(      "industrylq1.png" );
    cityTextures[ CST_INDUSTRY_L_Q2 ] = readTexture(      "industrylq2.png" );
    cityTextures[ CST_INDUSTRY_L_Q3 ] = readTexture(      "industrylq3.png" );
    cityTextures[ CST_INDUSTRY_L_Q4 ] = readTexture(      "industrylq4.png" );
    cityTextures[ CST_INDUSTRY_L_L1 ] = readTexture(      "industryll1.png" );
    cityTextures[ CST_INDUSTRY_L_L2 ] = readTexture(      "industryll2.png" );
    cityTextures[ CST_INDUSTRY_L_L3 ] = readTexture(      "industryll3.png" );
    cityTextures[ CST_INDUSTRY_L_L4 ] = readTexture(      "industryll4.png" );
    cityTextures[ CST_INDUSTRY_L_M1 ] = readTexture(      "industrylm1.png" );
    cityTextures[ CST_INDUSTRY_L_M2 ] = readTexture(      "industrylm2.png" );
    cityTextures[ CST_INDUSTRY_L_M3 ] = readTexture(      "industrylm3.png" );
    cityTextures[ CST_INDUSTRY_L_M4 ] = readTexture(      "industrylm4.png" );
    cityTextures[ CST_INDUSTRY_L_H1 ] = readTexture(      "industrylh1.png" );
    cityTextures[ CST_INDUSTRY_L_H2 ] = readTexture(      "industrylh2.png" );
    cityTextures[ CST_INDUSTRY_L_H3 ] = readTexture(      "industrylh3.png" );
    cityTextures[ CST_INDUSTRY_L_H4 ] = readTexture(      "industrylh4.png" );
    cityTextures[ CST_FARM_O0 ] = readTexture(            "farm0.png" );
    cityTextures[ CST_FARM_O1 ] = readTexture(            "farm1.png" );
    cityTextures[ CST_FARM_O2 ] = readTexture(            "farm2.png" );
    cityTextures[ CST_FARM_O3 ] = readTexture(            "farm3.png" );
    cityTextures[ CST_FARM_O4 ] = readTexture(            "farm4.png" );
    cityTextures[ CST_FARM_O5 ] = readTexture(            "farm5.png" );
    cityTextures[ CST_FARM_O6 ] = readTexture(            "farm6.png" );
    cityTextures[ CST_FARM_O7 ] = readTexture(            "farm7.png" );
    cityTextures[ CST_FARM_O8 ] = readTexture(            "farm8.png" );
    cityTextures[ CST_FARM_O9 ] = readTexture(            "farm9.png" );
    cityTextures[ CST_FARM_O10 ] = readTexture(           "farm10.png" );
    cityTextures[ CST_FARM_O11 ] = readTexture(           "farm11.png" );
    cityTextures[ CST_FARM_O12 ] = readTexture(           "farm12.png" );
    cityTextures[ CST_FARM_O13 ] = readTexture(           "farm13.png" );
    cityTextures[ CST_FARM_O14 ] = readTexture(           "farm14.png" );
    cityTextures[ CST_FARM_O15 ] = readTexture(           "farm15.png" );
    cityTextures[ CST_FARM_O16 ] = readTexture(           "farm16.png" );
}

/*
 * Process event
 */
void GameView::event(const Event& event)
{
    int stepx = (int) floor ( tileWidth / 2 );
    int stepy = (int) floor ( tileHeight / 2 );
    
    switch(event.type) {
        case Event::MOUSEBUTTONUP:
            if(!event.inside) {
                printf("notinside.\n");
                break;
            }
            printf("inside.\n");
            
            std::cout << "GameView::event click Button: " << event.mousebutton ;
            std::cout << "Pos " << event.mousepos.x << "/" << event.mousepos.y << "\n";
            if(event.mousebutton==SDL_BUTTON_RIGHT)
                recenter(event.mousepos);
            else
                click(event.mousepos);
            break;
        case Event::KEYUP:
            if( event.keysym.mod & KMOD_SHIFT ){
                stepx =  (int) 5 * tileWidth;
                stepy =  (int) 5 * tileHeight;
            } 
            if ( event.keysym.sym == SDLK_KP9 ) {
                viewport.x += stepx;
                viewport.y -= stepy;
            }
            if ( event.keysym.sym == SDLK_KP1 ) {
                viewport.x -= stepx;
                viewport.y += stepy;
            }
            if ( event.keysym.sym == SDLK_KP8 ) {
                viewport.y -= stepy;
            }
            if ( event.keysym.sym == SDLK_KP2 ) {
                viewport.y += stepy;
            }
            if ( event.keysym.sym == SDLK_KP7 ) {
                viewport.x -= stepx;
                viewport.y -= stepy;
            }
            if ( event.keysym.sym == SDLK_KP3 ) {
                viewport.x += stepx;
                viewport.y += stepy;
            }
            if ( event.keysym.sym == SDLK_KP6 ) {
                viewport.x += stepx;
            }
            if ( event.keysym.sym == SDLK_KP4 ) {
                viewport.x -= stepx;
            }
            if ( event.keysym.sym == SDLK_KP5 ) {
                viewport.x = floor ( ( virtualScreenWidth - getWidth()  ) / 2 );
                viewport.y = floor ( ( virtualScreenHeight- getHeight() ) / 2 );
            }
            break;
        default:
            break;
    }
}

/*
 * Process map-selection
 */
void GameView::click(const Vector2 &pos)
{
  Vector2 tile=getTile(pos);
  std::cout << "Tile-pos:"<<tile.x<<","<<tile.y<<std::endl;
  getMPS()->setView(tile.x,tile.y);
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
const void GameView::recenter(Vector2 pos)
{
    //sanity check
    
    //
    pos += viewport;
    viewport.x = floor( pos.x - ( getWidth() / 2 ) );
    viewport.y = floor( pos.y - ( getHeight() / 2 ) );
    
    //request redraw
}

/*
 * Find point on Screen, where lower right corner of tile
 * is placed.
 */
const Vector2 GameView::getScreenPoint(Vector2 tile)
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
const Vector2 GameView::getTile(Vector2 point)
{
    Vector2 tile;
    // Map Point to virtual Screen
    point += viewport;
    tile.x = ( point.x - virtualScreenWidth / 2 ) / tileWidth +  point.y  / tileHeight;
    tile.y =  2 * point.y  / tileHeight  - tile.x; 

    tile.x = floor( tile.x );
    tile.y = floor( tile.y );

    return tile;
}

/*
 *  Draw a Tile
 */
const void GameView::drawTile( Painter& painter, Vector2 tile )
{
    int tx = (int) tile.x;
    int ty = (int) tile.y;
    
    //prepare some pens
    Color black, white, red, green, blue;
    black.parse( "black" );
    white.parse( "white" );
    red.parse( "red" );
    green.parse( "green" );
    blue.parse( "blue" );

    Rect2D tilerect( 0, 0, tileWidth, tileHeight );
    Vector2 tileOnScreenPoint = getScreenPoint( tile );

    //is Tile in City? If not draw Blank
    if( tx < 0 || ty < 0 || tx > WORLD_SIDE_LEN || ty > WORLD_SIDE_LEN )
    {
        tileOnScreenPoint.x -= ( blankTexture->getWidth() / 2);
        tileOnScreenPoint.y -= blankTexture->getHeight(); 
        tilerect.move( tileOnScreenPoint );    
        tilerect.setSize( blankTexture->getWidth(), blankTexture->getHeight() );
        painter.drawTexture( blankTexture, tilerect );
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
    
    texture = cityTextures[ MP_TYPE( upperLeftX, upperLeftY ) ];
    
    if( texture )
    {
        tileOnScreenPoint.x -= ( texture->getWidth() / 2);
        tileOnScreenPoint.y -= texture->getHeight(); 
        tilerect.move( tileOnScreenPoint );    
        tilerect.setSize( texture->getWidth(), texture->getHeight() );
        painter.drawTexture( texture, tilerect );
    }
    else 
    {
    //std::cout << "drawTile Texture Missing " << MP_TYPE( upperLeftX, upperLeftY ) << "\n";
        tileOnScreenPoint.x =  floor( tileOnScreenPoint.x - ( tileWidth / 2));
        tileOnScreenPoint.y -= tileHeight; 
        tilerect.move( tileOnScreenPoint );    
        painter.setFillColor( red );
        painter.fillRectangle( tilerect );    
    }
}

/*
 *  Paint an isometric View of the City in the component.
 */
void GameView::draw( Painter& painter )
{
    //std::cout << "\nGameView::draw()\n";
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
}

//Register as Component
IMPLEMENT_COMPONENT_FACTORY(GameView)
