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

  setView(10,10);
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


void LCMps::setView(int x,int y)
{

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
            getSound()->playwav( "Blacksmith" );
	        mps_blacksmith (mps_x, mps_y);
		break;
	    case (GROUP_COALMINE):
            getSound()->playwav( "CoalMine" );
		    mps_coalmine (mps_x, mps_y);
		break;
	    case GROUP_COAL_POWER:
		mps_coal_power (mps_x, mps_y);
		break;
	    case (GROUP_COMMUNE):
	        mps_commune (mps_x, mps_y);
		break;
	    case (GROUP_CRICKET):
            getSound()->playwav( "SportsCroud" );
		    mps_cricket (mps_x, mps_y);
		break;
	    case (GROUP_FIRESTATION):
            getSound()->playwav( "FireStation" );
	        mps_firestation (mps_x, mps_y);
		break;
	    case (GROUP_HEALTH):
		mps_health_centre (mps_x, mps_y);
		break;
	    case (GROUP_INDUSTRY_H):
            getSound()->playwav( "IndustryHigh" );
		    mps_heavy_industry (mps_x, mps_y);
		break;
	    case (GROUP_INDUSTRY_L):
            getSound()->playwav( "IndustryLight" );
		    mps_light_industry (mps_x, mps_y);
		break;
	    case (GROUP_MILL):
	        mps_mill (mps_x, mps_y);
		break;
	    case (GROUP_MONUMENT):
	        mps_monument (mps_x, mps_y);
		break;
	    case (GROUP_OREMINE):
	        mps_oremine (mps_x, mps_y);
		break;
	    case GROUP_ORGANIC_FARM: 
		mps_organic_farm(mps_x, mps_y);
		break;
	    case (GROUP_PORT):
	        mps_port (mps_x, mps_y);
		break;
	    case (GROUP_POTTERY):
	        mps_pottery (mps_x, mps_y);
		break;
	    case GROUP_POWER_LINE:
            getSound()->playwav( "PowerLine" );
	        mps_power_line (mps_x, mps_y);
	        break;
	    case (GROUP_RAIL):
            getSound()->playwav( "RailTrain" );
		    mps_rail (mps_x, mps_y);
		break;
	    case (GROUP_RECYCLE):
	        mps_recycle (mps_x, mps_y);
		break;
	    case GROUP_RESIDENCE_LL:
	    case GROUP_RESIDENCE_LH:
            getSound()->playwav( "ResidentialLow" );
		    mps_residence(mps_x, mps_y);
		break;
	    case GROUP_RESIDENCE_ML:
	    case GROUP_RESIDENCE_MH:
            getSound()->playwav( "ResidentialMed" );
		    mps_residence(mps_x, mps_y);
		break;
	    case GROUP_RESIDENCE_HL:
	    case GROUP_RESIDENCE_HH:
            getSound()->playwav( "ResidentialHigh" );
		    mps_residence(mps_x, mps_y);
		break;
	    case (GROUP_ROAD):
            getSound()->playwav( "TraficLow" );
            //getSound()->playwav( "TraficHigh" );
            //TODO: find out when to use TraficHigh
		    mps_road (mps_x, mps_y);
		break;
	    case (GROUP_ROCKET):
	        mps_rocket (mps_x, mps_y);
		break;
	    case (GROUP_SCHOOL):
            getSound()->playwav( "School" );
	        mps_school (mps_x, mps_y);
		break;
	    case GROUP_SOLAR_POWER:
	        mps_solar_power (mps_x, mps_y);
	        break;
	    case (GROUP_SUBSTATION):
            getSound()->playwav( "Substation" );
	        mps_substation (mps_x, mps_y);
	        break;
	    case (GROUP_TIP):
	        mps_tip (mps_x, mps_y);
		break;
	    case (GROUP_TRACK):
            getSound()->playwav( "DirtTrack" );
		    mps_track(mps_x, mps_y);
		break;
	    case (GROUP_MARKET):
		mps_market (mps_x, mps_y);
		break;
	    case (GROUP_UNIVERSITY):
	        mps_university (mps_x, mps_y);
	        break;
	    case (GROUP_WATER):
	        mps_water (mps_x, mps_y);
	    break;
	    case (GROUP_WINDMILL):
	        mps_windmill (mps_x, mps_y);
		break;
	    default: 
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
