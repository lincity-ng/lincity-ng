#ifndef __GAMEVIEW_HPP__
#define __GAMEVIEW_HPP__

#include "gui/Component.hpp"
#include "gui/XmlReader.hpp"
#include "gui/Vector2.hpp"
#include "gui/Texture.hpp"
#include <time.h>
#include <SDL.h> 
#include <SDL_thread.h>
#include <SDL_image.h>

#include "lincity/engglobs.h"
#include "MapPoint.hpp"
#include "MiniMap.hpp"

class GameView : public Component
{
public:
    GameView();
    ~GameView();

    void parse(XmlReader& reader);
    
    void draw(Painter& painter);
    void resize(float width, float height );
    void event(const Event& event);

    void requestRedraw();

    //Show City Tile(x/y) by centering the screen 
    void show(MapPoint point);
    MapPoint getCenter();
        
    //Set Zoomlevel to 100 Percent
    void resetZoom();
    //increase Zoomlevel 
    void zoomIn();
    //decrease Zoomlevel 
    void zoomOut();

    //size in Tiles of marking under Cursor
    void setCursorSize( int size ); 

    //inform GameView about change in Mini Map Mode
    void setMapMode( MiniMap::DisplayMode mMode );
private:
    void recenter(const Vector2& pos);
    Vector2 getScreenPoint(MapPoint point);
    MapPoint getTile(const Vector2& point);
    void drawTile(Painter& painter, MapPoint point);
    void drawOverlay(Painter& painter, MapPoint point);
    void fillDiamond( Painter& painter, const Rect2D& rect );
    void drawDiamond( Painter& painter, const Rect2D& rect );
    void loadTextures();
    static int gameViewThread(void* data);
    
    void setZoom(float newzoom);
    Texture* readTexture(const std::string& filename);
    SDL_Surface* readImage(const std::string& filename);
    void preReadCityTexture(int textureType, const std::string& filename);

    static const float defaultTileWidth = 128;
    static const float defaultTileHeight = 64;

    float tileWidth, tileHeight, zoom; 
    //a virtual screen containing the whole city
    float virtualScreenWidth, virtualScreenHeight;

    //upper left corner of the viewport on virtual screen
    Vector2 viewport;

    //check if tile is in city
    bool inCity( MapPoint tile );
    
    Texture* cityTextures[ NUM_OF_TYPES ];
    SDL_Surface* cityImages[ NUM_OF_TYPES ];
    Texture* blankTexture;
    int cityTextureX[ NUM_OF_TYPES ];
    int cityTextureY[ NUM_OF_TYPES ];
    SDL_mutex* mTextures;
    SDL_mutex* mThreadRunning;
    SDL_Thread* loaderThread;
    volatile bool stopThread;
        
    MapPoint tileUnderMouse;
    Vector2 dragStart;
    bool mouseInGameView;
    bool dragging, middleButtonDown;
    Uint32 dragStartTime;
    
    bool roadDragging, leftButtonDown;
    MapPoint startRoad;

    bool hideHigh;
    int mapOverlay;
    MiniMap::DisplayMode mapMode;
    static const int overlayNone = 0;
    static const int overlayOn = 1;
    static const int overlayOnly = 2;
    static const int overlayMAX = 2;

    static const int gameAreaMin = 1;
    static const int gameAreaMax = WORLD_SIDE_LEN -2;
    void markTile( Painter& painter, MapPoint map );

    int cursorSize;
};

GameView* getGameView();

#endif

