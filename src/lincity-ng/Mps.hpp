#ifndef __lc_mps_h__
#define __lc_mps_h__

#include "gui/Component.hpp"
#include "MapPoint.hpp"
#include "gui_interface/mps.h"

class XmlReader;
static const int paragraphCount = 13;

void mps_update(int mps_x,int mps_y,int mps_style);
extern int mps_x;
extern int mps_y;
extern int mps_style;

class LCMps : public Component
{
public:
  LCMps();
  ~LCMps();

  void parse(XmlReader& reader);

  void setView(MapPoint point, int style = MPS_MAP );
  
  void setText(int i,const std::string &s);
  void playBuildingSound( int mps_x, int mps_y );
 private:
  int mX,mY;
  //Number of Paragraphs in mps.xml
  //<Paragraph name="mps_text13" style="mps_text">Text13</Paragraph>

  //  std::vector<AGText*> mTexts;
};

LCMps *getMPS();

#endif
