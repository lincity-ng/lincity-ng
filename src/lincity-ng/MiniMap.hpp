/* ---------------------------------------------------------------------- *
 * src/lincity-ng/MiniMap.hpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2005      David Kamphausen <david.kamphausen@web.de>
 * Copyright (C) 2024      David Bears <dbear4q@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
** ---------------------------------------------------------------------- */

#ifndef __lc_minimap_h__
#define __lc_minimap_h__

#include <memory>                   // for unique_ptr
#include <string>                   // for string, basic_string
#include <vector>                   // for vector

#include "gui/Color.hpp"            // for Color
#include "gui/Component.hpp"        // for Component
#include "gui/Vector2.hpp"          // for Vector2
#include "lincity/MapPoint.hpp"     // for MapPoint
#include "lincity/commodities.hpp"  // for Commodity

class Button;
class CheckButton;
class Game;
class Texture;
class World;
class XmlReader;

class MiniMap : public Component
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

    void switchView(const std::string& viewname);
    void scrollPageDown(bool down);

    Commodity getStuffID();
    void toggleStuffID(int step);

    void mapViewChangeDisplayMode(DisplayMode mode);

    void setGame(Game *game);

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

    void constrainPosition();

    Game *game = nullptr;
    World& getWorld() const;

    MapPoint upperLeft, lowerRight;

    DisplayMode mMode;
    Commodity stuff_ID;
    int tilesize;
    int border;
    int left, top; //Positioning of minimap

    std::vector<CheckButton*> switchButtons;
    std::unique_ptr<Texture> mTexture;

    bool mFullRefresh;
    bool alreadyAttached;
    bool inside;
    // used for the middle mouse button popup to remember last visible tab
    std::string lastTabName;
};

MiniMap* getMiniMap();

#endif

/** @file lincity-ng/MiniMap.hpp */
