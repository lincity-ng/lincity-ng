#include <config.h>

#include "Mps.hpp"
#include "gui_interface/mps.h"
#include "lincity/engglobs.h"
#include "lincity/modules/all_modules.h"

#include "Util.hpp"

#include "gui/XmlReader.hpp"
#include "gui/ComponentFactory.hpp"
#include "gui/ComponentLoader.hpp"
#include "gui/Paragraph.hpp"

#include "Sound.hpp"

LCMps *mLCMPS = 0;

LCMps::LCMps()
{
    assert(mLCMPS == 0);
    mLCMPS = this;
}

LCMps::~LCMps()
{
    if(mLCMPS == this)
        mLCMPS = 0;
}

void
LCMps::parse(XmlReader& reader)
{
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* name = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(name, value)) {
            continue;
        } else {
            std::cerr << "Unknown attribute '" << name 
                      << "' skipped in LCMps.\n";
        }
    }

  Component* component = parseEmbeddedComponent(reader);
  addChild(component);

  width = component->getWidth();
  height = component->getHeight();

  setView(MapPoint(10,10));
}

void LCMps::setText(int i,const std::string &s)
{
  if(i>paragraphCount) {
    std::cerr<<"setText("<<i<<") '"<< s << "' failed."<<std::endl;
    return;
  }
    std::ostringstream compname;
    compname << "mps_text" << (i+1);
    Paragraph* p = getParagraph(*this, compname.str());
    if(p)
    {
      p->setText(s);
     }
    else
      std::cerr<<"Paragraph with num:"<<i<<" not found"<<std::endl;
}


void LCMps::setView(MapPoint point, int style /* = MPS_MAP */ )
{
    int x = point.x;
    int y = point.y;
    if( x < 0 || y < 0 || x >= WORLD_SIDE_LEN || y >= WORLD_SIDE_LEN )
        return;

    int xx,yy;
  
    xx=x;
    yy=y;
    if (MP_TYPE(x,y) == CST_USED)
    {
        xx = MP_INFO(x,y).int_1;
        yy = MP_INFO(x,y).int_2;
    }
    
    if( style == MPS_ENV ){
        mps_update(x , y, style); 
    } else {
        mps_update(xx,yy,style); //MPS_GLOBAL);// MPS_ENV);// MPS_MAP);
    }
}

LCMps *getMPS()
{
  return mLCMPS;
}

void LCMps::playBuildingSound( int mps_x, int mps_y  ){
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
                getSound()->playSound( "SubstationOn" );
            } else if( MP_TYPE( mps_x, mps_y ) == CST_SUBSTATION_G ){
                getSound()->playSound( "SubstationOff" );
            } else{   //CST_SUBSTATION_RG
                getSound()->playSound( "Substation" );
            }
            break;
        case GROUP_TIP:
            getSound()->playSound( "Tip" );
            break;
        case GROUP_TRACK:
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
            if( MP_INFO(mps_x, mps_y ).int_2 < MODERN_WINDMILL_TECH ){ 
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
            if( MP_TYPE( mps_x, mps_y ) == CST_GREEN ){
                getSound()->playSound( "Green" );
            }  
    }
}

int mps_set_silent( int x, int y, int style );

void mps_update(int mps_x, int mps_y,int mps_style)
{
    // first clear all text
    for(int i=0;i<paragraphCount;i++)
        mps_store_title(i,"");
    
    mps_set_silent( mps_x, mps_y, mps_style);
    mps_refresh();
}

IMPLEMENT_COMPONENT_FACTORY(LCMps)
INTERN_LCMpsFactory myLCMpsFactory;
