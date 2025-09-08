/* ---------------------------------------------------------------------- *
 * src/lincity-ng/GameView.hpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2005      Wolfgang Becker <uafr@gmx.de>
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

#ifndef __GAMEVIEW_HPP__
#define __GAMEVIEW_HPP__

#include <SDL.h>                  // for Uint32, SDL_Surface, SDL_Thread
#include <filesystem>             // for path
#include <memory>                 // for unique_ptr
#include <string>                 // for string, basic_string

#include "gui/Component.hpp"      // for Component
#include "gui/Vector2.hpp"        // for Vector2
#include "lincity/MapPoint.hpp"   // for MapPoint
#include "lincity/resources.hpp"  // for GraphicsInfo

class Button;
class Game;
class Painter;
class Rect2D;
class Texture;
class UserOperation;
class World;
namespace xmlpp {
class TextReader;
}  // namespace xmlpp

class GameView : public Component
{
public:
    GameView();
    ~GameView();

    void parse(xmlpp::TextReader& reader);

    void draw(Painter& painter);
    void resize(float width, float height );
    void event(const Event& event);
    void setDirty() { Component::setDirty(); }
    void setMapDirty() { refreshMap = true; }

    //Show City Tile(x/y) by centering the screen
    void show(MapPoint point, bool redraw = true );
    MapPoint getCenter();

    //Set Zoomlevel to default
    void resetZoom();
    //increase Zoomlevel
    void zoomIn();
    //decrease Zoomlevel
    void zoomOut();

    //scroll the map
    void scroll(float elapsedTime);

    //size in Tiles of marking under Cursor
    void setCursorSize( int size );

    //Show informations about selected Tool (and price to build several tiles)
    void showToolInfo( int number = 0 );

    //evaluate main_screen_originx and main_screen_originy
    void readOrigin( bool redraw = true );
    //set main_screen_originx and main_screen_originy
    void writeOrigin();

    void printStatusMessage( std::string message );

    void fetchTextures(void);
    //convert all remaining images in ConstructionGroup::reourceMap to textures

    //check if tile is in city
    bool inCity( MapPoint tile );

    void setGame(Game *game);

    bool textures_ready;
    //bool economyGraph_open;
    int remaining_images;

private:
    void connectButtons();
    void buttonClicked( Button* button );
    Vector2 getScreenPoint(MapPoint point);
    MapPoint getTile(const Vector2& point);
    void drawTile(Painter& painter, const MapPoint &point);
    void drawTexture(Painter& painter, const MapPoint &point, GraphicsInfo *graphicsInfo);
    void drawOverlay(Painter& painter, const MapPoint &point);
    void fillDiamond( Painter& painter, const Rect2D& rect );
    void drawDiamond( Painter& painter, const Rect2D& rect );
    static int gameViewThread(void* data);
    void viewportUpdated();
    void setZoom(float newzoom);
    void zoomMouse(float factor, Vector2 mousepos);
    bool constrainViewportPosition(bool useScrollCorrection);
    SDL_Surface* readImage(const std::filesystem::path& filename);
    void preReadImages(void);
    Texture* readTexture(const std::filesystem::path& filename);
    //void loadTextures();
    //void preReadCityTexture(int textureType, const std::string& filename);

    float tileWidth, tileHeight, zoom;

    Game *game = nullptr;
    World& getWorld() const;
    UserOperation *getUserOperation() const;

    enum {
        SCROLL_NONE = 0x0,
        SCROLL_UP = 1,
        SCROLL_DOWN = (1 << 1),
        SCROLL_LEFT = (1 << 2),
        SCROLL_RIGHT = (1 << 3),
        SCROLL_UP_LEFT = (1 << 4),
        SCROLL_UP_RIGHT = (1 << 5),
        SCROLL_DOWN_LEFT = (1 << 6),
        SCROLL_DOWN_RIGHT = (1 << 7),
        SCROLL_LSHIFT = (1 << 8),
        SCROLL_RSHIFT = (1 << 9),
        SCROLL_UP_ALL = SCROLL_UP | SCROLL_UP_LEFT | SCROLL_UP_RIGHT,
        SCROLL_DOWN_ALL = SCROLL_DOWN | SCROLL_DOWN_LEFT | SCROLL_DOWN_RIGHT,
        SCROLL_LEFT_ALL = SCROLL_LEFT | SCROLL_UP_LEFT | SCROLL_DOWN_LEFT,
        SCROLL_RIGHT_ALL = SCROLL_RIGHT | SCROLL_UP_RIGHT | SCROLL_DOWN_RIGHT,
        SCROLL_SHIFT_ALL = SCROLL_LSHIFT | SCROLL_RSHIFT,
    };
    int keyScrollState;
    int mouseScrollState;

    //upper left corner of the viewport on virtual screen
    Vector2 viewport;

    int bulldozeCost( MapPoint tile );
    int buildCost( MapPoint tile );

    GraphicsInfo blankGraphicsInfo;

    //SDL_mutex* mTextures;
    //SDL_mutex* mThreadRunning;
    SDL_Thread* loaderThread;
    bool stopThread;

    MapPoint tileUnderMouse;
    bool mouseInGameView;
    bool dragging;
    Vector2 scrollCorrection;

    bool roadDragging, ctrDrag, leftButtonDown;
    // NOTE: leftButtonDown indicates whether the middle button is down
    //       (I didn't bother to refactor the name.)
    MapPoint startRoad;
    bool areaBulldoze;

    static const float defaultTileWidth;
    static const float defaultTileHeight;
    static const float defaultZoom;

    bool hideHigh, showTerrainHeight;
    int mapOverlay;
    static const int overlayNone = 0;
    static const int overlayOn = 1;
    static const int overlayOnly = 2;
    static const int overlayMAX = 2;

    void markTile(Painter& painter, MapPoint tile);

    int cursorSize;
    bool buttonsConnected;

    std::unique_ptr<Texture> mapTexture;
    bool refreshMap;

    MapPoint realTile( MapPoint tile );
    std::string lastStatusMessage;

    void setPanningCursor();
    void setDefaultCursor();
};

Uint32 autoScroll( Uint32 interval, void *param );
static const int scrollBorder = 5;

#endif


/** @file lincity-ng/GameView.hpp */
