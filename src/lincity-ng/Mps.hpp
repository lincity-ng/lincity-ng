#ifndef __lc_mps_h__
#define __lc_mps_h__

#include "gui/Component.hpp"
#include <SDL_mixer.h>


class XmlReader;

class LCMps : public Component
{
public:
  LCMps();
  ~LCMps();

  void parse(XmlReader& reader);

  void setView(int x,int y);
  
  void setText(int i,const std::string &s);

  void playwav( int id );
 private:
  int mX,mY;
  static const int maxWaves = 15;
  Mix_Chunk* waves[ maxWaves ];
  bool audioOpen;

  //  std::vector<AGText*> mTexts;
};

LCMps *getMPS();

#endif
