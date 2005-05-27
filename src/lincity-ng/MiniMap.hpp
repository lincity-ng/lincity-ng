#ifndef __lc_minimap_h__
#define __lc_minimap_h__

#include "gui/Component.hpp"
#include "gui/Color.hpp"
#include "lincity/lin-city.h"
#include "gui/Texture.hpp"
#include "MapPoint.hpp"

class XmlReader;
class Button;
class CheckButton;

class MiniMap:public Component
{
public:
    enum DisplayMode {NORMAL,POLLUTION,UB40,STARVE,POWER,FIRE,CRICKET,HEALTH,COAL,TRAFFIC,MAX};

    MiniMap();
    ~MiniMap();

    void parse(XmlReader& reader);
  
    virtual void draw(Painter &painter);
    virtual void event(const Event& event);
  
    void setGameViewCorners(const MapPoint& upperLeft,
            const MapPoint& upperRight, const MapPoint& lowerRight,
            const MapPoint& lowerLeft);

    Color getColor(int x,int y) const;
    Color getColorNormal(int x,int y) const;
    void showMpsEnv( MapPoint tile );
    void hideMpsEnv();

    void switchView(const std::string& viewname);

private:
    void mapViewButtonClicked(CheckButton* button, int);
    void speedButtonClicked(CheckButton* button, int);
    void zoomInButtonClicked(Button* button);
    void zoomOutButtonClicked(Button* button);
    
    void switchButton(CheckButton* button, int);
    void switchMapViewButton(const std::string &pName);
    
    void attachButtons();
    Component *findRoot(Component *c);
    Vector2 mapPointToVector(MapPoint p);
  
    Vector2 gameViewPoints[ 4 ];
    short mappointoldtype[WORLD_SIDE_LEN][WORLD_SIDE_LEN];

    DisplayMode mMode;

    int tilesize;
    int border;

    std::vector<CheckButton*> switchButtons;
    std::auto_ptr<Texture> mTexture;
    
    int mpsXOld, mpsYOld, mpsStyleOld;

    bool mFullRefresh;
    bool alreadyAttached;
    bool inside;
    std::string shownTabName;
};

MiniMap* getMiniMap();

#endif
