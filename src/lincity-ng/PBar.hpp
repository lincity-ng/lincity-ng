#ifndef __lc_pbar_h__
#define __lc_pbar_h__

#include <vector>
#include <sstream>
#include <iomanip>
#include "gui_interface/pbar_interface.h"
#include "gui/Component.hpp"

class LCPBar;
class XmlReader;
class Component;

extern LCPBar *LCPBarInstance;

class LCPBar : public Component
{
public:
  LCPBar(Component *widget, XmlReader &reader);
  ~LCPBar();
  
  void setValue(int num, int value);
};

#endif
