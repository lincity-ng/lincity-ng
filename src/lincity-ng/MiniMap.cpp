#include "MiniMap.hpp"

#include "gui/Painter.hpp"
#include "gui/Rect2D.hpp"
#include "gui/ComponentFactory.hpp"
#include "gui/XmlReader.hpp"
#include "gui/Event.hpp"
#include "gui_interface/mps.h"

#include "lincity/lin-city.h"
#include "lincity/engglobs.h"
#include "lincity/lctypes.h"

#include "gui_interface/shared_globals.h"
#include "gui_interface/screen_interface.h"

#include <set>
#include <iostream>

#include "Debug.hpp"

#define LC_MOUSE_LEFTBUTTON 1
#define LC_MOUSE_RIGHTBUTTON 2
#define LC_MOUSE_MIDDLEBUTTON 3

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

MiniMap::MiniMap(Component *parent, XmlReader& reader):
  Component(parent),mMode(NORMAL),tilesize(2),mTexture(0)
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
}

void MiniMap::draw(Painter &painter)
{

  int x, y;
  short typ, grp;

  // simple and bad implementation
  // FIXME: should be stored SDL_Surface and then blitted
  // SDL_Surface should be updated, only if needed

  Painter mpainter(mTexture.get());

  for(y=0;y<WORLD_SIDE_LEN && y<height/tilesize;y++)
    for(x=0;x<WORLD_SIDE_LEN && x<width/tilesize;x++)
      {
	typ = MP_TYPE(x,y);
	if (typ != mappointoldtype[x][y] || mFullRefresh)
	  {
	    mappointoldtype[x][y] = typ;
	    
	    grp = get_group_of_type(typ);

	    Color mc=getColor(x,y);
	    mpainter.setFillColor(mc);
	    mpainter.fillRectangle(Rect2D(x*tilesize,y*tilesize,(x+main_groups[grp].size)*tilesize+1,(y+main_groups[grp].size)*tilesize));
	  }
      }


  painter.drawTexture(mTexture.get(),Rect2D(0,0,width,height));

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

/*
bool MiniMap::eventMouseClick(const AGEvent *m)
{
#ifdef NORMAL
  char *mode[]={"NORMAL","POLLUTION","UB40","STARVE","POWER","FIRE","CRICKET","HEALTH","COAL","MAX"};
  
  TRACE;
  ++(int)mMode;
  if(mMode==MAX)
    mMode=NORMAL;
  cdebug("MODE:"<<mMode<<":"<<mode[mMode]);
  mText->setText(mode[mMode]);
  return false;
#else
#ifdef MPS_CHECK
  const AGSDLEvent *e=static_cast<const AGSDLEvent*>(m);

  CTRACE;
  if(e)
    {
      CTRACE;
      AGPoint p=e->getMousePosition();
      p=p-getRect().getPosition();

      cdebug("p:"<<p.x<<"/"<<p.y);
      p.x/=tilesize;
      p.y/=tilesize;

      if(p.x>=0 && p.x<WORLD_SIDE_LEN && p.y>=0 && p.y<WORLD_SIDE_LEN)
	{
	  int xx=p.x;
	  int yy=p.y;
	  if (MP_TYPE(p.x,p.y) == CST_USED)
	    {
	      xx = MP_INFO(p.x,p.y).int_1;
	      yy = MP_INFO(p.x,p.y).int_2;
	    }
	  
	  
	  
	  getMPS()->setView(xx,yy);
	}
    }
  return false;
#else
  const AGSDLEvent *e=static_cast<const AGSDLEvent*>(m);
      AGPoint p=e->getMousePosition();
      p=p-getRect().getPosition();

      cdebug("p:"<<p.x<<"/"<<p.y);
      p.x/=tilesize;
      p.y/=tilesize;

      if(e->getButton()==SDL_BUTTON_RIGHT)
	do_mouse_main_win(p.x,p.y,LC_MOUSE_RIGHTBUTTON);
      else
	do_mouse_main_win(p.x,p.y,LC_MOUSE_LEFTBUTTON);


#endif

#endif
}
*/

static MiniMap::DisplayMode getNextMode(MiniMap::DisplayMode mode)
{
    switch(mode) {
        case MiniMap::NORMAL:
            return MiniMap::POLLUTION;
        case MiniMap::POLLUTION:
            return MiniMap::UB40;
        case MiniMap::UB40:
            return MiniMap::STARVE;
        case MiniMap::STARVE:
            return MiniMap::POWER;
        case MiniMap::POWER:
            return MiniMap::FIRE;
        case MiniMap::FIRE:
            return MiniMap::CRICKET;
        case MiniMap::CRICKET:
            return MiniMap::HEALTH;
        case MiniMap::HEALTH:
            return MiniMap::COAL;
        case MiniMap::COAL:
            return MiniMap::NORMAL;
        default:
            assert(false);
    }

    return MiniMap::NORMAL;
}

void MiniMap::event(const Event& event)
{
  if(event.type==Event::MOUSEBUTTONDOWN)
    {
      if(event.mousepos.x>=0 && event.mousepos.y>=0 && event.mousepos.x<width && event.mousepos.y<height)
	{
	  CTRACE;
	  cdebug("mousePos:"<<event.mousepos.x<<","<<event.mousepos.y);
          mMode = getNextMode(mMode);
	  mFullRefresh=true;
	  //      cdebug("MODE:"<<mMode<<":"<<mode[mMode]);
	  //      mText->setText(mode[mMode]);
	}
    }
}


IMPLEMENT_COMPONENT_FACTORY(MiniMap)

INTERN_MiniMapFactory myMiniMapFactory;
