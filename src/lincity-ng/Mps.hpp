#ifndef __lc_mps_h__
#define __lc_mps_h__

#include "gui/Component.hpp"

//#include "ag_table.h"
//#include "ag_text.h"
class XmlReader;

class LCMps:public Component
{
 public:
  LCMps(Component *pWidget,XmlReader &reader);

  void setView(int x,int y);
  
  void setText(int i,const std::string &s);

 private:
  int mX,mY;

  //  std::vector<AGText*> mTexts;
};

LCMps *getMPS();

#endif
