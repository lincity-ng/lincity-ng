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

#include <SDL_mixer.h>

LCMps *mLCMPS = 0;

void mps_update(int,int,int);

LCMps::LCMps()
{
    assert(mLCMPS == 0);
    mLCMPS = this;

    audioOpen = false;
   	/* Open the audio device */
	if (Mix_OpenAudio( MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
	} else {
		audioOpen = true;
        memset(&waves, 0, sizeof(waves));
        
        //Load Waves
        //
        //TODO: there has to be a better way to organize the Sounds.
        waves[ 0 ] = Mix_LoadWAV("data/sounds/Click.wav");
        waves[ 1 ] = Mix_LoadWAV("data/sounds/School3.wav");
        waves[ 3 ] = Mix_LoadWAV("data/sounds/RailTrain3.wav");
        waves[ 4 ] = Mix_LoadWAV("data/sounds/TraficLow3.wav");
	}

}

LCMps::~LCMps()
{
    if(mLCMPS == this)
        mLCMPS = 0;
    for ( int i = 0; i < maxWaves; i++ )
        if( waves[i] ) {
            Mix_FreeChunk(  waves[i] );
             waves[i] = 0;
    }
	if ( audioOpen ) {
		Mix_CloseAudio();
		audioOpen = false;
	}

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

/*
 *  Playback an Audio-Effect
 */
void LCMps::playwav( int id ) {
    printf("Audio %i request...", id);
    
    if( !audioOpen ){
		printf("Can't play Audio.\n");
        return;
    }
    if( waves[ id ] ) {
        Mix_PlayChannel(0, waves[ id ], 0); 
        printf("Oky\n");
    }
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
            getMPS()->playwav( 3 );
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
            getMPS()->playwav( 1 );
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
            getMPS()->playwav( 4 );
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
