#include <config.h>

#include "Mps.hpp"
#include "gui_interface/mps.h"
#include "lincity/engglobs.h"
#include "lincity/modules/all_modules.h"

#include "gui/XmlReader.hpp"

LCMps *mLCMPS;

void mps_update(int,int,int);

LCMps::LCMps(Component *parent,XmlReader &reader):
  Component(parent)
{
  
  mLCMPS=this;
  /*
  int i;
  addFixedColumn();
  for(i=0;i<10;i++)
    {
      addFixedRow();
    }

  AGFont f("Arial.ttf",13);
  f.setColor(AGColor(0,0,0));

  AGText *t;
  for(int i=0;i<10;i++)
    {
      mTexts.push_back(t=new AGText(this,AGPoint(0,0),"nothing",f));
      AGTable::addChild(0,i,t);
    }


    arrange();*/
}

void LCMps::setText(int i,const std::string &s)
{
  /*
  cdebug(i<<":"<<s);
  if(i>=0 && i<10)
    mTexts[i]->setText(s);
    arrange();*/
}





void LCMps::setView(int x,int y)
{
  /*
  CTRACE;
  cdebug(x<<"/"<<y);

  // first clear all text
  for(int i=0;i<10;i++)
    setText(i,"");
  mps_update(x,y,MPS_GLOBAL);// MPS_ENV);// MPS_MAP);

  //  if(x<0 || y<0)
  */
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
	        mps_blacksmith (mps_x, mps_y);
		break;
	    case (GROUP_COALMINE):
		mps_coalmine (mps_x, mps_y);
		break;
	    case GROUP_COAL_POWER:
		mps_coal_power (mps_x, mps_y);
		break;
	    case (GROUP_COMMUNE):
	        mps_commune (mps_x, mps_y);
		break;
	    case (GROUP_CRICKET):
		mps_cricket (mps_x, mps_y);
		break;
	    case (GROUP_FIRESTATION):
	        mps_firestation (mps_x, mps_y);
		break;
	    case (GROUP_HEALTH):
		mps_health_centre (mps_x, mps_y);
		break;
	    case (GROUP_INDUSTRY_H):
		mps_heavy_industry (mps_x, mps_y);
		break;
	    case (GROUP_INDUSTRY_L):
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
	        mps_power_line (mps_x, mps_y);
	        break;
	    case (GROUP_RAIL):
		mps_rail (mps_x, mps_y);
		break;
	    case (GROUP_RECYCLE):
	        mps_recycle (mps_x, mps_y);
		break;
	    case GROUP_RESIDENCE_LL:
	    case GROUP_RESIDENCE_ML:
	    case GROUP_RESIDENCE_HL:
	    case GROUP_RESIDENCE_LH:
	    case GROUP_RESIDENCE_MH:
	    case GROUP_RESIDENCE_HH:
		mps_residence(mps_x, mps_y);
		break;
	    case (GROUP_ROAD):
		mps_road (mps_x, mps_y);
		break;
	    case (GROUP_ROCKET):
	        mps_rocket (mps_x, mps_y);
		break;
	    case (GROUP_SCHOOL):
	        mps_school (mps_x, mps_y);
		break;
	    case GROUP_SOLAR_POWER:
	        mps_solar_power (mps_x, mps_y);
	        break;
	    case (GROUP_SUBSTATION):
	        mps_substation (mps_x, mps_y);
	        break;
	    case (GROUP_TIP):
	        mps_tip (mps_x, mps_y);
		break;
	    case (GROUP_TRACK):
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
