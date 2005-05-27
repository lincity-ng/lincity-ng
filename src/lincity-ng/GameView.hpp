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
    void show(MapPoint point, bool redraw = true );
    MapPoint getCenter();
        
    //Set Zoomlevel to default
    void resetZoom();
    //increase Zoomlevel 
    void zoomIn();
    //decrease Zoomlevel 
    void zoomOut();

    //scroll the map
    void scroll( void );

    //size in Tiles of marking under Cursor
    void setCursorSize( int size ); 

    //inform GameView about change in Mini Map Mode
    void setMapMode( MiniMap::DisplayMode mMode );

    //Show informations about selected Tool (and price to build several tiles)
    void showToolInfo( int number = 0 );

    //evaluate main_screen_originx and main_screen_originy
    void readOrigin( bool redraw = true );
    //set main_screen_originx and main_screen_originy
    void writeOrigin();    

    void printStatusMessage( std::string message );
private:
    void connectButtons();
    void buttonClicked( Button* button );
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

    float tileWidth, tileHeight, zoom; 
    //a virtual screen containing the whole city
    float virtualScreenWidth, virtualScreenHeight;

    enum {
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
    };
    int keyScrollState;
    int mouseScrollState;

    //upper left corner of the viewport on virtual screen
    Vector2 viewport;

    //check if tile is in city
    bool inCity( MapPoint tile );

    int bulldozeCost( MapPoint tile );
    
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
    bool dragging, rightButtonDown;
    Uint32 dragStartTime;
    
    bool roadDragging, leftButtonDown;
    MapPoint startRoad;
    
    static const float defaultTileWidth;
    static const float defaultTileHeight;
    static const float defaultZoom;

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
    bool buttonsConnected;

    MapPoint realTile( MapPoint tile );
    std::string lastStatusMessage;
};

GameView* getGameView();
  
Uint32 autoScroll( Uint32 interval, void *param );
static const int scrollBorder = 5;

#endif

