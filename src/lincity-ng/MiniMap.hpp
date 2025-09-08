/* ---------------------------------------------------------------------- *
 * src/lincity-ng/MiniMap.hpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2005      David Kamphausen <david.kamphausen@web.de>
 * Copyright (C) 2024-2025 David Bears <dbear4q@gmail.com>
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
#include "gui/Signal.hpp"
#include "gui/Vector2.hpp"          // for Vector2
#include "lincity/commodities.hpp"  // for Commodity

class Button;
class CheckButton;
class Game;
class MapPoint;
class MapTile;
class Painter;
class Texture;
class World;
namespace xmlpp {
class TextReader;
}  // namespace xmlpp

class MiniMap : public Component
{
public:
    enum DisplayMode  {NORMAL,POLLUTION,UB40,STARVE,POWER,FIRE,CRICKET,HEALTH,COAL,TRAFFIC,COMMODITIES,MAX};

    MiniMap();
    ~MiniMap();

    void parse(xmlpp::TextReader& reader);

    virtual void draw(Painter &painter);
    virtual void event(const Event& event);

    void setGameViewCorners(Vector2 upperLeft, Vector2 lowerRight);

    Color getColor(MapTile& tile) const;
    Color getColorNormal(MapTile& tile) const;

    void switchView(const std::string& viewname);
    void scrollPageDown(bool down);

    Commodity getStuffID();
    void toggleStuffID(int step);

    void mapViewChangeDisplayMode(DisplayMode mode);

    void setGame(Game *game);

    void setDirty() { Component::setDirty(); }
    void setMapDirty() { mFullRefresh = true; }

    Signal<> mapChanged;

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
    void updateStatusMessage();

    void constrainPosition();

    Game *game = nullptr;
    World& getWorld() const;

    Vector2 upperLeft, lowerRight;

    DisplayMode mMode;
    Commodity stuff_ID;
    int tilesize;
    int border;
    Vector2 anchor; //Positioning of minimap

    std::vector<CheckButton*> switchButtons;
    std::unique_ptr<Texture> mTexture;
    void refreshTexture(Painter &painter);

    bool mFullRefresh;
    bool alreadyAttached;
    bool inside;
    // used for the middle mouse button popup to remember last visible tab
    std::string lastTabName;
};

MiniMap* getMiniMap();

#endif

/** @file lincity-ng/MiniMap.hpp */
