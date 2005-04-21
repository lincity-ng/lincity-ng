#ifndef __lc_mps_h__
#define __lc_mps_h__

#include "gui/Component.hpp"
#include "MapPoint.hpp"

class XmlReader;

class LCMps : public Component
{
public:
  LCMps();
  ~LCMps();

  void parse(XmlReader& reader);

  void setView(MapPoint point);
  
  void setText(int i,const std::string &s);
 private:
  int mX,mY;

  //  std::vector<AGText*> mTexts;
};

LCMps *getMPS();

#endif
