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
    void show( const int x, const int y );
    void show( const Vector2 pos );

    //Set Zoomlevel to 100 Percent
    void resetZoom();
    //increase Zoomlevel 
    void zoomIn();
    //decrease Zoomlevel 
    void zoomOut();

    //size in Tiles of marking under Cursor
    void setCursorSize( int size ); 
private:
    const void recenter(const Vector2& pos);
    const Vector2 getScreenPoint(const Vector2& tile);
    const Vector2 getTile(const Vector2& point);
    const void drawTile(Painter& painter, const Vector2& tile);
    const void fillDiamond( Painter& painter, const Rect2D rect );
    const void drawDiamond( Painter& painter, const Rect2D rect );
    const void loadTextures();
    static int gameViewThread( void* );
    
    void setZoom(const int newzoom);
    Texture* readTexture(const std::string& filename);
    SDL_Surface* readImage(const std::string& filename);
    void preReadCityTexture( int textureType, const std::string& filename );

    static const int defaultTileWidth = 128;
    static const int defaultTileHeight = 64;
    static const int defaultZoom = 1000;

    int tileWidth, tileHeight, zoom; 
    //a virtual screen containing the whole city
    int virtualScreenWidth, virtualScreenHeight;

    //upper left corner of the viewport on virtual screen
    Vector2 viewport;
    
    Texture* cityTextures[ NUM_OF_TYPES ];
    SDL_Surface* cityImages[ NUM_OF_TYPES ];
    Texture* blankTexture;
    int cityTextureX[ NUM_OF_TYPES ];
    int cityTextureY[ NUM_OF_TYPES ];
    SDL_mutex* mTextures;
    SDL_mutex* mThreadRunning;
    SDL_Thread* loaderThread;
    volatile bool stopThread;
        
    Vector2 tileUnderMouse, dragStart;
    bool mouseInGameView;
    bool dragging, rightButtonDown;
    Uint32 dragStartTime;

    int cursorSize;
};

GameView* getGameView();

#endif

