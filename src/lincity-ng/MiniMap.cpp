#include <config.h>

#include "MiniMap.hpp"
#include "GameView.hpp"
#include "MainLincity.hpp"

#include "gui/Painter.hpp"
#include "gui/Button.hpp"
#include "gui/Rect2D.hpp"
#include "gui/ComponentFactory.hpp"
#include "gui/XmlReader.hpp"
#include "gui/Event.hpp"
#include "gui_interface/mps.h"

#include "lincity/lin-city.h"
#include "lincity/engglobs.h"
#include "lincity/lctypes.h"

#include "gui/callback/Callback.hpp"

#include "gui_interface/shared_globals.h"
#include "gui_interface/screen_interface.h"

#include <set>
#include <iostream>

#include "Debug.hpp"
#include "Util.hpp"

#define LC_MOUSE_LEFTBUTTON 1
#define LC_MOUSE_RIGHTBUTTON 2
#define LC_MOUSE_MIDDLEBUTTON 3
  
char *buttonNames[]={"MapViewNormal","MapViewUB40","MapViewPollution","MapViewFood",
    "MapViewPower","MapViewFire","MapViewSport","MapViewHealth","MapViewCoal",
    "ZoomInButton","ZoomOutButton","SpeedPauseButton","SpeedSlowButton",
    "SpeedNormalButton","SpeedFastButton"};
#define BUTTON_COUNT 15

Uint8 brightness(const Color &c)
{
  Uint16 x=c.r;
  x+=c.g;
  x+=c.b;
  return x/3;
}

Color makeGrey(const Color &c)
{
  Uint8 b=brightness(c);
  return Color(b,b,b);
}

Color light(const Color &c,Uint8 b)
{
  return Color(((Uint16(c.r)*b)>>8),
	       ((Uint16(c.g)*b)>>8),
	       ((Uint16(c.b)*b)>>8));
}

MiniMap* miniMapPtr = 0;

MiniMap* getMiniMap()
{
    return miniMapPtr;
}
    
MiniMap::MiniMap()
    : mMode(NORMAL), tilesize(2), mTexture(0), border(0)
{   
    assert( miniMapPtr == 0 );
    miniMapPtr = this;
    gameViewPoints[ 0 ].x = 0;
    gameViewPoints[ 0 ].y = 0;
    gameViewPoints[ 1 ].x = 0;
    gameViewPoints[ 1 ].y = 0;
    gameViewPoints[ 2 ].x = 0;
    gameViewPoints[ 2 ].y = 0;
    gameViewPoints[ 3 ].x = 0;
    gameViewPoints[ 3 ].y = 0;
}

MiniMap::~MiniMap() {
    if(miniMapPtr == this)
        miniMapPtr = 0;
}

void
MiniMap::parse(XmlReader& reader)
{
     // parse attributes...
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* name = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(strcmp(name, "width") == 0) {
            if(sscanf(value, "%f", &width) != 1) {
                std::stringstream msg;
                msg << "Couldn't parse width attribute (" << value << ").";
                throw std::runtime_error(msg.str());
            }
        } else if(strcmp(name, "height") == 0) {
            if(sscanf(value, "%f", &height) != 1) {
                std::stringstream msg;
                msg << "Couldn't parse height attribute (" << value << ").";
                throw std::runtime_error(msg.str());
            }
        } else if(strcmp(name, "border") == 0) {
            if(sscanf(value, "%d", &border) != 1) {
                std::stringstream msg;
                msg << "Couldn't parse border attribute (" << value << ").";
                throw std::runtime_error(msg.str());
            }
        } else if(strcmp(name, "tilesize") == 0) {
            if(sscanf(value, "%d", &tilesize) != 1) {
                std::stringstream msg;
                msg << "Couldn't parse tilesize attribute (" << value << ").";
                throw std::runtime_error(msg.str());
            }
        } else {
            std::cerr << "Unknown attribute '" << name << "' skipped.\n";
        }
    }
    if(width <= 0 || height <= 0)
      throw std::runtime_error("Width or Height invalid");
    

    // create alpha-surface
    SDL_Surface* image = SDL_CreateRGBSurface(0, (int) width, (int) height, 32,
					      0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);

    SDL_Surface *copy = SDL_DisplayFormat(image);
    SDL_FreeSurface(image);
    image=copy;
    
    mTexture.reset(texture_manager->create(image)); // don't delete image, as Texture-class takes ownage

    mFullRefresh=true;
  
  alreadyAttached=false;
}

Component *MiniMap::findRoot(Component *c)
{
  while(c->getParent())
    return findRoot(c->getParent());
  return c;
}

void MiniMap::attachButtons()
{
    if(alreadyAttached)
    return;
  alreadyAttached=true;
  
  Component *root=findRoot(this);
  
  for(int i=0;i<BUTTON_COUNT;i++)
    {
      Button* b = getButton(*root, buttonNames[i]);
      std::cout<<"Button1 "<<buttonNames[i]<<" found!"<<std::endl;
      b->clicked.connect(makeCallback(*this, &MiniMap::chooseButtonClicked));
    } 
}

void MiniMap::chooseButtonClicked(Button* button)
{
  std::cout<<"MiniMap::button '" << button->getName() << "' clicked"<<std::endl;
  std::string name=button->getName();
  for(int i=0;i<BUTTON_COUNT;i++)
  {
    if(name==buttonNames[i])
     {
       switch(i)
       {
        case 0: mMode=NORMAL;break;
        case 1: mMode=UB40;break;
        case 2: mMode=POLLUTION;break;
        case 3: mMode=STARVE;break;
        case 4: mMode=POWER;break;
        case 5: mMode=FIRE;break;
        case 6: mMode=CRICKET;break;
        case 7: mMode=HEALTH;break;
        case 8: mMode=COAL;break;
        case 9: getGameView()->zoomIn(); break;
        case 10:getGameView()->zoomOut() ;break;
        case 11:setLincitySpeed( 0 ); break;
        case 12:setLincitySpeed( SLOW_TIME_FOR_YEAR ) ; break;
        case 13:setLincitySpeed( MED_TIME_FOR_YEAR ) ; break;
        case 14:setLincitySpeed( FAST_TIME_FOR_YEAR ) ; break;
       }
     }
   }
   mFullRefresh=true;
}

/*
 *  Set the Corners of the GameView to show in Minimap
 */
void MiniMap::setGameViewCorners( const Vector2 &upperLeft, const Vector2 &upperRight, 
          const Vector2 &lowerRight, const Vector2 &lowerLeft )
{
    gameViewPoints[ 0 ] = upperLeft * tilesize;
    gameViewPoints[ 1 ] = upperRight * tilesize;
    gameViewPoints[ 2 ] = lowerRight * tilesize;
    gameViewPoints[ 3 ] = lowerLeft * tilesize;
    setDirty();
}

void MiniMap::draw(Painter &painter)
{
  attachButtons();

  int x, y;
  short typ, grp;

  // simple and bad implementation
  // FIXME: should be stored SDL_Surface and then blitted
  // SDL_Surface should be updated, only if needed

  std::auto_ptr<Painter> mpainter (painter.createTexturePainter(mTexture.get()));
  Color alphawhite( 255, 255, 255, 128 );
  if(mpainter.get() == 0)
  {
    // workaround - so that it works with GL, too, as long as there's no TexturePainter for this
  
    for(y=0;y<WORLD_SIDE_LEN && y<height/tilesize;y++)
      for(x=0;x<WORLD_SIDE_LEN && x<width/tilesize;x++)
        {
          typ = MP_TYPE(x,y);
          if (typ != mappointoldtype[x][y] || mFullRefresh)
          {
            mappointoldtype[x][y] = typ;
            grp = get_group_of_type(typ);
  
            Color mc=getColor(x,y);
            painter.setFillColor(mc);
            painter.fillRectangle(Rect2D(x*tilesize,y*tilesize,(x+main_groups[grp].size)*tilesize+1,(y+main_groups[grp].size)*tilesize));
          }
        }
    //show current GameView
    painter.setLineColor( alphawhite );
    painter.drawPolygon( 4, gameViewPoints );    
    return;
  }

  for(y=0;y<WORLD_SIDE_LEN && y<height/tilesize;y++)
    for(x=0;x<WORLD_SIDE_LEN && x<width/tilesize;x++)
      {
        typ = MP_TYPE(x,y);
        if (typ != mappointoldtype[x][y] || mFullRefresh)
        {
          mappointoldtype[x][y] = typ;
          grp = get_group_of_type(typ);

          Color mc=getColor(x,y);
          mpainter->setFillColor(mc);
          mpainter->fillRectangle(Rect2D(x*tilesize,y*tilesize,(x+main_groups[grp].size)*tilesize+1,(y+main_groups[grp].size)*tilesize));
        }
      }
    //show current GameView
    mpainter->setLineColor( alphawhite );
    mpainter->drawPolygon( 4, gameViewPoints );    


  painter.drawTexture(mTexture.get(), Vector2(0, 0));

  mFullRefresh=false;
}

Color MiniMap::getColorNormal(int x,int y) const
{
  int xx,yy;
  
  xx=x;
  yy=y;
  if (MP_TYPE(x,y) == CST_USED)
    {
      xx = MP_INFO(x,y).int_1;
      yy = MP_INFO(x,y).int_2;
    }


  short typ = MP_TYPE(xx,yy);
  int mc=typ+20;
  
  return Color((mc&3)*64,((mc>>2)&3)*64,((mc>>4)&3)*64); // some random color
}

Color MiniMap::getColor(int x,int y) const
{
  int xx,yy;
  
  xx=x;
  yy=y;
  if (MP_TYPE(x,y) == CST_USED)
    {
      xx = MP_INFO(x,y).int_1;
      yy = MP_INFO(x,y).int_2;
    }

  int flags=MP_INFO(xx,yy).flags;

  switch(mMode)
    {
    case NORMAL:
      return getColorNormal(x,y);
    case POLLUTION:
      {
	static std::set<short> pset;
	short p=MP_POL(x,y);
	if(pset.find(p)==pset.end())
	  {
	    pset.insert(p);
	  }
	float v=p/600.0;
	if(v<0)
	  v=0;
	if(v>1)
	  v=1;
	Color mc((int) (0xFF*v), (int) (0xFF*(1-v)), 0);
	mc=light(mc,brightness(getColorNormal(x,y)));
	return mc;
      }
    case FIRE:
    case CRICKET:
    case HEALTH:
      if( ((flags&FLAG_FIRE_COVER) && mMode==FIRE) ||
	  ((flags&FLAG_CRICKET_COVER) && mMode==CRICKET) ||
	  ((flags&FLAG_HEALTH_COVER) && mMode==HEALTH))
	{
	  Color mc(0,0xFF,0);
	  mc=light(mc,brightness(getColorNormal(x,y)));
	  return mc;
	}
      else
	return makeGrey(getColorNormal(x,y));
    case UB40:
      {
	
	if (MP_GROUP_IS_RESIDENCE(xx,yy)) 
	  {
	    if (MP_INFO(xx,yy).int_1 < -20)
	      return Color(0xFF,0,0);
	    else if (MP_INFO(xx,yy).int_1 < 10)
	      return Color(0x7F,0,0);
	    else
	      return Color(0,0xFF,0);
	  } 
	else 
	  {
	    return makeGrey(getColorNormal(x,y));//AGColor(0,0x7F,0);
	    //	    col = green (14);
	  }
      }
    case COAL:
      {
	Color c(0x77,0,0);
	if(MP_INFO(x,y).coal_reserve==0)
	  return makeGrey(getColorNormal(x,y));
	else if (MP_INFO(x,y).coal_reserve >= COAL_RESERVE_SIZE / 2)
	  return Color(0,0xFF,0);
	else if (MP_INFO(x,y).coal_reserve < COAL_RESERVE_SIZE / 2)
	  return Color(0xFF,0,0);

	return c;
      }
    case STARVE:
      {
	if (MP_GROUP_IS_RESIDENCE(xx,yy)) {
	  if ((total_time - MP_INFO(x,y).int_2) < 20)
	    return Color(0xFF,0,0);
	  else if ((total_time - MP_INFO(x,y).int_2) < 100)
	      return Color(0x7F,0,0);
	  else
	      return Color(0,0xFF,0);
	} else {
	  return makeGrey(getColorNormal(x,y));
	}
      }
    case POWER:
      {
	Color mc;
	if (get_power (xx, yy, 1, 1) != 0) {
	  mc=Color(0,0xFF,0);
	} else if (get_power (xx, yy, 1, 0) != 0) {
	  mc=Color(0,0x7F,0);
	} else {
	  mc=Color(0xFF,0xFF,0xFF);
	}
	  mc=light(mc,brightness(getColorNormal(x,y)));
	return mc;
      }
    case MAX:
      std::cerr<<"Undefined MiniMap-Display-type!"<<std::endl;
    };
  return Color(0xFF,0,0xFF);
}

void MiniMap::event(const Event& event)
{
  if(event.type==Event::MOUSEBUTTONDOWN && event.inside)
    {
        cdebug("mousePos:"<<event.mousepos.x<<","<<event.mousepos.y);
        // move main-map
        // get Tile, that was clicked
        int tilex = (int) ((event.mousepos.x - border ) / tilesize);
        int tiley = (int) ((event.mousepos.y - border ) / tilesize);
        //cdebug( "click on tile "<< tilex <<"/"<< tiley );
        getGameView()->show( tilex, tiley );
        //mMode = getNextMode(mMode);
        mFullRefresh=true;
        //      cdebug("MODE:"<<mMode<<":"<<mode[mMode]);
        //      mText->setText(mode[mMode]);
    }
}

IMPLEMENT_COMPONENT_FACTORY(MiniMap)
