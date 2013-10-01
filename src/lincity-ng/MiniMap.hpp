/*
Copyright (C) 2005 David Kamphausen <david.kamphausen@web.de>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef __lc_minimap_h__
#define __lc_minimap_h__

#include "gui/Component.hpp"
#include "gui/Color.hpp"
#include "gui/Texture.hpp"
#include "MapPoint.hpp"
#include "lincity/lintypes.h" //for knowing Construction
#include <memory>

class XmlReader;
class Button;
class CheckButton;

class MiniMap:public Component
{
public:
    enum DisplayMode  {NORMAL,POLLUTION,UB40,STARVE,POWER,FIRE,CRICKET,HEALTH,COAL,TRAFFIC,COMMODITIES,MAX};

    MiniMap();
    ~MiniMap();

    void parse(XmlReader& reader);

    virtual void draw(Painter &painter);
    virtual void event(const Event& event);

    void setGameViewCorners(
        const MapPoint& upperLeft, const MapPoint& lowerRight
    );

    Color getColor(int x,int y) const;
    Color getColorNormal(int x,int y) const;
    void showMpsEnv( MapPoint tile );
    void hideMpsEnv();

    void switchView(const std::string& viewname);
    void scrollPageDown(bool down);

    Construction::Commodities getStuffID();
    void toggleStuffID(int step);

private:
    void mapViewButtonClicked(CheckButton* button, int);
    void speedButtonClicked(CheckButton* button, int);
    void zoomInButtonClicked(Button* button);
    void zoomOutButtonClicked(Button* button);
    void scrollPageDownButtonClicked(Button* button);
    void scrollPageUpButtonClicked(Button* button);


    void switchButton(CheckButton* button, int);
    void switchMapViewButton(const std::string &pName);

    void attachButtons();
    Component *findRoot(Component *c);
//FIXME
    Vector2 mapPointToVector(MapPoint p);

    MapPoint upperLeft, lowerRight;

    DisplayMode mMode;
    Construction::Commodities stuff_ID;
    int tilesize;
    int border;
    int left, top; //Positioning of minimap

    std::vector<CheckButton*> switchButtons;
    std::auto_ptr<Texture> mTexture;

    int mpsXOld, mpsYOld, mpsStyleOld;

    bool mFullRefresh;
    bool alreadyAttached;
    bool inside;
    // used for the middle mouse button popup to remember last visible tab
    std::string lastTabName;
};

MiniMap* getMiniMap();

#endif

/** @file lincity-ng/MiniMap.hpp */

