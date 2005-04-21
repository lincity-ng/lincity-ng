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

void mps_update(int,int,int);

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
  Component* component = parseEmbeddedComponent(reader);
  addChild(component);

  width = component->getWidth();
  height = component->getHeight();

  setView(MapPoint(10,10));
}

void LCMps::setText(int i,const std::string &s)
{
  if(i>=10)
    return;
    std::ostringstream compname;
    compname << "mps_text" << (i+1);
    Paragraph* p = getParagraph(*this, compname.str());
    if(p)
    {
      p->setText(s);
     }
    else
      std::cout<<"Paragraph with num:"<<i<<" not found"<<std::endl;
}


void LCMps::setView(MapPoint point)
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

  // first clear all text
  for(int i=0;i<10;i++)
    setText(i," ");
  mps_update(xx,yy,MPS_MAP);//GLOBAL);// MPS_ENV);// MPS_MAP);
}


LCMps *getMPS()
{
  return mLCMPS;
}

void mps_update(int mps_x,int mps_y,int mps_style)
{
    switch (mps_style) {
    case MPS_MAP:
    {
      //cdebug(MP_GROUP(mps_x,mps_y));
      switch(MP_GROUP(mps_x, mps_y)) 
        {
        case (GROUP_BLACKSMITH):
            getSound()->playSound( "Blacksmith" );
            mps_blacksmith (mps_x, mps_y);
        break;
        case (GROUP_COALMINE):
            getSound()->playSound( "CoalMine" );
            mps_coalmine (mps_x, mps_y);
        break;
        case GROUP_COAL_POWER:
            //TODO: play which sound?
            //getSound()->playSound( "PowerCoalEmpty" );
            //getSound()->playSound( "PowerCoalFull" );
            //getSound()->playSound( "PowerCoalLow" );
            getSound()->playSound( "PowerCoalMed" );
            mps_coal_power (mps_x, mps_y);
        break;
        case (GROUP_COMMUNE):
            getSound()->playSound( "Commune" );
            mps_commune (mps_x, mps_y);
        break;
        case (GROUP_CRICKET):
            getSound()->playSound( "SportsCroud" );
            mps_cricket (mps_x, mps_y);
        break;
        case (GROUP_FIRESTATION):
            getSound()->playSound( "FireStation" );
            mps_firestation (mps_x, mps_y);
        break;
        case (GROUP_HEALTH):
            getSound()->playSound( "Health" );
            mps_health_centre (mps_x, mps_y);
        break;
        case (GROUP_INDUSTRY_H):
            getSound()->playSound( "IndustryHigh" );
            mps_heavy_industry (mps_x, mps_y);
        break;
        case (GROUP_INDUSTRY_L):
            getSound()->playSound( "IndustryLight" );
            mps_light_industry (mps_x, mps_y);
        break;
        case (GROUP_MILL):
            getSound()->playSound( "Mill" );
            mps_mill (mps_x, mps_y);
        break;
        case (GROUP_MONUMENT):
            if ((MP_INFO( mps_x,mps_y).int_1 * 100 / BUILD_MONUMENT_JOBS) >= 100) {
                getSound()->playSound( "Monument" );
            } else {
                getSound()->playSound( "MonumentConstruction" );
            }
            mps_monument (mps_x, mps_y);
        break;
        case (GROUP_OREMINE):
            getSound()->playSound( "OreMine" );
            mps_oremine (mps_x, mps_y);
        break;
        case GROUP_ORGANIC_FARM: 
            getSound()->playSound( "OrganicFarm" );
            mps_organic_farm(mps_x, mps_y);
        break;
        case (GROUP_PORT):
            getSound()->playSound( "Harbor" );
            mps_port (mps_x, mps_y);
        break;
        case (GROUP_POTTERY):
            getSound()->playSound( "Pottery" );
            mps_pottery (mps_x, mps_y);
        break;
        case GROUP_POWER_LINE:
            getSound()->playSound( "PowerLine" );
            mps_power_line (mps_x, mps_y);
            break;
        case (GROUP_RAIL):
            getSound()->playSound( "RailTrain" );
            mps_rail (mps_x, mps_y);
        break;
        case (GROUP_RECYCLE):
            getSound()->playSound( "Recycle" );
            mps_recycle (mps_x, mps_y);
        break;
        case GROUP_RESIDENCE_LL:
            getSound()->playSound( "ResidentialLowLow" );
            mps_residence(mps_x, mps_y);
        break;
        case GROUP_RESIDENCE_LH:
            getSound()->playSound( "ResidentialLow" );
            mps_residence(mps_x, mps_y);
        break;
        case GROUP_RESIDENCE_ML:
        case GROUP_RESIDENCE_MH:
            getSound()->playSound( "ResidentialMed" );
            mps_residence(mps_x, mps_y);
        break;
        case GROUP_RESIDENCE_HL:
        case GROUP_RESIDENCE_HH:
            getSound()->playSound( "ResidentialHigh" );
            mps_residence(mps_x, mps_y);
        break;
        case (GROUP_ROAD):
            getSound()->playSound( "TraficLow" );
            //getSound()->playSound( "TraficHigh" );
            //TODO: find out when to use TraficHigh
            mps_road (mps_x, mps_y);
        break;
        case (GROUP_ROCKET):
            getSound()->playSound( "Rocket" );
            //TODO: what about RocketExplosion RocketTakeoff
            mps_rocket (mps_x, mps_y);
        break;
        case (GROUP_SCHOOL):
            getSound()->playSound( "School" );
            mps_school (mps_x, mps_y);
        break;
        case GROUP_SOLAR_POWER:
            getSound()->playSound( "PowerSolar" );
            mps_solar_power (mps_x, mps_y);
            break;
        case (GROUP_SUBSTATION):
            getSound()->playSound( "Substation" );
            mps_substation (mps_x, mps_y);
            break;
        case (GROUP_TIP):
            getSound()->playSound( "Tip" );
            mps_tip (mps_x, mps_y);
        break;
        case (GROUP_TRACK):
            getSound()->playSound( "DirtTrack" );
            mps_track(mps_x, mps_y);
        break;
        case (GROUP_MARKET):
            //TODO: which one?
            //getSound()->playSound( "MarketEmpty" );
            //getSound()->playSound( "MarketFull" );
            //getSound()->playSound( "MarketLow" );
            getSound()->playSound( "MarketMed" );
            mps_market (mps_x, mps_y);
        break;
        case (GROUP_UNIVERSITY):
            getSound()->playSound( "University" );
            mps_university (mps_x, mps_y);
        break;
        case (GROUP_WATER):
            getSound()->playSound( "Water" );
            mps_water (mps_x, mps_y);
        break;
        case (GROUP_WINDMILL):
            if( MP_INFO(mps_x, mps_y ).int_2 < MODERN_WINDMILL_TECH ){ 
                getSound()->playSound( "WindMill" );
            } else {
                getSound()->playSound( "WindMillHTech" );
            }
            mps_windmill (mps_x, mps_y);
        break;
        default: 
            if( MP_TYPE( mps_x, mps_y ) == CST_GREEN ){
                getSound()->playSound( "Green" );
                mps_store_title(0,"Green");
                mps_store_title(4, "build something here" );
            }
            //no special information on this group, just show the Name.
            mps_store_title(0, main_groups[ MP_GROUP( mps_x, mps_y ) ].name );
            mps_store_title(2, "no further information available" );
            
            printf("MPS unimplemented for that module\n");
            mps_style = MPS_NONE;
        }
    }
        break;
    case MPS_ENV:
    mps_right (mps_x, mps_y);
    break;
    case MPS_GLOBAL: 
    {
        switch (mps_global_style) {
        case MPS_GLOBAL_FINANCE:
        mps_global_finance();
        break;
        case MPS_GLOBAL_OTHER_COSTS:
        mps_global_other_costs();
        break;
        case MPS_GLOBAL_HOUSING:
        mps_global_housing();
        break;
        default:
        printf("MPS unimplemented for global display\n");
        break;
        }
    }
    break;
    }

    mps_refresh();
}

IMPLEMENT_COMPONENT_FACTORY(LCMps)
INTERN_LCMpsFactory myLCMpsFactory;
