#ifndef __alternate_h__
#define __alternate_h__

#include <vector>
#include <sstream>
#include <iomanip>
#include "gui_interface/pbar_interface.h"
#include "gui/Component.hpp"

#include <list>

class XmlReader;
class Component;

class Alternate : public Component
{
public:
  Alternate(Component *widget, XmlReader &reader);
  virtual ~Alternate();
  
  void toggleDisplay();
  virtual void draw(Painter &painter);
private:
  Component *current;
};
#endif
