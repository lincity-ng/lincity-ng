#ifndef __lc_isodisplay_h__
#define __lc_isodisplay_h__

#include "gui/Component.hpp"
#include "gui/Color.hpp"
#include "lincity/lin-city.h"
#include "gui/Texture.hpp"

#include <map>

class XmlReader;

class IsoDisplay:public Component
{
 public:

  enum DisplayMode {NORMAL,POLLUTION,UB40,STARVE,POWER,FIRE,CRICKET,HEALTH,COAL,MAX};

  IsoDisplay(Component *parent, XmlReader& reader);
  
  virtual ~IsoDisplay();
  
  // colors must be corrected somehow - maybe get them from oldgui/screen.cc
  virtual void draw(Painter &painter);

  virtual void event(const Event& event);

 private:

  Texture *getTexture(int x,int y); // get Texture from cache or load from disk
  SDL_Surface *loadTexture(short t); // load texture from disk

  short mappointoldtype[WORLD_SIDE_LEN][WORLD_SIDE_LEN];

  DisplayMode mMode;

  std::map<short,Texture*> mTextures;
  
  std::auto_ptr<Texture> mTexture;

  bool mFullRefresh;
  
  int tileSize; // basic tile-size
  int px,py;
};

#endif
