#include "IsoDisplay.hpp"

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

#define SUPPORT_BIGGER_TILES

IsoDisplay::IsoDisplay(Component *parent, XmlReader& reader):
  Component(parent),tileSize(64),px(0),py(0)
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
    
    mFullRefresh=true;
    mTexture.reset(texture_manager->create(image)); // don't delete image, as Texture-class takes ownage
}

IsoDisplay::~IsoDisplay()
{
  std::map<short,Texture*>::iterator i=mTextures.begin();
  for(;i!=mTextures.end();i++)
    delete i->second;
}


void IsoDisplay::draw(Painter &painter)
{
  int sw=width,sh=height;
  int x, y;
  int i, j;
  short typ, grp;

  Painter mpainter(mTexture.get());

  int tw=tileSize,tw2=tileSize/2,th=tileSize/2,th2=tileSize/4;
  
  mFullRefresh=true;
  
  // place for map-middle
  int sx,sy;
  sx=WORLD_SIDE_LEN/2-(sh/th2-sw/tw2)/2;
  sy=WORLD_SIDE_LEN/2-(sh/th2+sw/tw2)/2;
  
  // begin one left/up
  for(i=-3;i<sh/th2+3;i++)
  {
    for(j=-7;j<sw/tw2;j++)
      {
        int mi,mj;
        mi=i+py;
        mj=j+px;
        if(((i+j)%2)==0)
        {
          continue; // between tiles
         }
        x=(mj-mi)/2;
        y=(mi+mj)/2;
        
        x+=sx;
        y+=sy;
        
        if(x>=0 && y>=0 && x<WORLD_SIDE_LEN && y<WORLD_SIDE_LEN)
        {
          typ = MP_TYPE(x,y);
          if (typ != mappointoldtype[x][y] || mFullRefresh)
            {
              int tx=j*tw2;
              int ty=i*th2;
              mappointoldtype[x][y] = typ;
              Texture *tex=getTexture(x,y);

              // for CST_USED              
              if(tex)      
              {
#ifdef SUPPORT_BIGGER_TILES
                if(tex->getWidth()>tw)
                {
                  ty-=(tex->getHeight()-th)/2;
                }
#endif
                mpainter.drawTexture(tex,Rect2D(tx,ty,tx+tw,ty+th));
              }
            }
        }
      }
   }
  painter.drawTexture(mTexture.get(),Rect2D(0,0,width,height));

  mFullRefresh=false;
}


// generate some random colored iso-tile
SDL_Surface *IsoDisplay::loadTexture(short t)
{
  int width=tileSize;
  int height=tileSize/2;

#ifdef SUPPORT_BIGGER_TILES
  short grp = get_group_of_type(t);

  width*=main_groups[grp].size;
  height*=main_groups[grp].size;
#endif

  SDL_Surface* image = SDL_CreateRGBSurface(0, (int) width, (int) height, 32,
                0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);

  Painter p(image);
    
  int mc=t+20;
  
  Color mc2((mc&3)*64,((mc>>2)&3)*64,((mc>>4)&3)*64, 100); // some random color

  p.setFillColor(mc2);
    
  for(int x=0;x<width;x++)
    for(int y=0;y<height;y++)
      if(abs(x-width/2)/2+abs(y-height/2)<=height/2)
        p.fillRectangle(Rect2D(x,y,x+1,y+1));

  SDL_SaveBMP(image,"test.bmp");
          
  return image;
}

Texture *IsoDisplay::getTexture(int x,int y)
{
  short t=MP_TYPE(x,y);
  
#ifdef SUPPORT_BIGGER_TILES
  if(t==CST_USED)
    return 0;
#endif  
  if(mTextures[t]==0)
  {
    SDL_Surface *image=loadTexture(t);
    mTextures[t]=texture_manager->create(image); // don't delete image, as Texture-class takes ownage
  }
  return mTextures[t];
}

void IsoDisplay::event(const Event& event)
{
  int border=20;

  if(event.type==Event::MOUSEBUTTONDOWN)
    {
      if(event.mousepos.x>=0 && event.mousepos.y>=0 && event.mousepos.x<width && event.mousepos.y<height)
      {
        if(event.mousepos.x<border)
          px-=2;
        if(event.mousepos.x>width-border)
          px+=2;
        if(event.mousepos.y<border)
          py-=2;
        if(event.mousepos.y>height-border)
          py+=2;
      }
    }
}


IMPLEMENT_COMPONENT_FACTORY(IsoDisplay)

INTERN_IsoDisplayFactory myIsoDisplayFactory;
