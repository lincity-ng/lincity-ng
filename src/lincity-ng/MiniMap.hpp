#ifndef __lc_minimap_h__
#define __lc_minimap_h__

#include "gui/Component.hpp"
#include "gui/Color.hpp"
//#include "ag_text.h"
#include "lincity/lin-city.h"

// this is an example for a minimap

class XmlReader;

class MiniMap:public Component
{
 public:

  enum DisplayMode {NORMAL,POLLUTION,UB40,STARVE,POWER,FIRE,CRICKET,HEALTH,COAL,MAX};

  MiniMap(Component *parent, XmlReader& reader);
  
  // colors must be corrected somehow - maybe get them from oldgui/screen.cc
  virtual void draw(Painter &painter);

  virtual void event(Event& event);

 private:

  Color getColor(int x,int y) const;
  Color getColorNormal(int x,int y) const;


  short mappointoldtype[WORLD_SIDE_LEN][WORLD_SIDE_LEN];

  DisplayMode mMode;

  //AGText *mText;

  int tilesize;
  int border;
};

#endif
