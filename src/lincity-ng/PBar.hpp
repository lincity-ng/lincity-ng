#ifndef __lc_pbar_h__
#define __lc_pbar_h__

#include <vector>
#include <sstream>
#include <iomanip>
#include "gui_interface/pbar_interface.h"
#include "gui/Component.hpp"

#include <map>

class LCPBar;
class XmlReader;
class Component;

extern LCPBar *LCPBarInstance;

class LCPBar : public Component
{
public:
  LCPBar();
  ~LCPBar();

  void parse(XmlReader& reader);
  
  void setValue(int num, int value);
private:
  std::map<int,int> oldValues;
};

class BarView : public Component
{
  public:
   BarView();
   ~BarView();

   void parse(XmlReader& reader);
   
   void setValue(float v);
   virtual void draw(Painter &painter);
  private:
   float value;
   bool dir;
};

#endif
