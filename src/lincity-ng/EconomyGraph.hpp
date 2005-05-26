#ifndef __EconomyGraph_HPP
#define __EconomyGraph_HPP

#include "gui/Component.hpp"
#include "gui/XmlReader.hpp"

class EconomyGraph : public Component {
public:
    EconomyGraph();
    ~EconomyGraph();

    void parse(XmlReader& reader);
    void draw(Painter& painter);
    void updateData();    
    void newFPS( int frame );
private:
    static const int border = 5;
    void drawHistoryLineGraph( Painter& painter, Rect2D mg );
    void drawSustBarGraph( Painter& painter, Rect2D mg );
    void drawFPSGraph( Painter& painter, Rect2D fpsRect );
 
    int* fps;
};

EconomyGraph* getEconomyGraph();

#endif
