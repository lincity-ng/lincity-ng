#ifndef __lc_mps_h__
#define __lc_mps_h__

#include "gui/Component.hpp"
#include "MapPoint.hpp"
#include "gui_interface/mps.h"

class XmlReader;
static const int MPS_PARAGRAPH_COUNT = 13;

void mps_update(int mps_x, int mps_y, int mps_style);
extern int mps_x;
extern int mps_y;
extern int mps_style;

class Paragraph;

/**
 * The MPS component is more or less a text component with 13 lines that 
 * is used to display status information about the game/engine.
 */
class Mps : public Component
{
public:
    Mps();
    ~Mps();

    void parse(XmlReader& reader);

    void clear();
    void setView(MapPoint point, int style = MPS_MAP );
  
    void setText(int i, const std::string &s);
    void playBuildingSound(int mps_x, int mps_y);

private:
    typedef std::vector<Paragraph*> Paragraphs;
    Paragraphs paragraphs;
    int mX, mY;
};

extern Mps* globalMPS;
extern Mps* mapMPS;
extern Mps* envMPS;

#endif
