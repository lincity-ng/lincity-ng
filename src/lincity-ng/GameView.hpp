#ifndef __GAMEVIEW_HPP__
#define __GAMEVIEW_HPP__

#include "gui/Component.hpp"
#include "gui/XmlReader.hpp"
#include "gui/Vector2.hpp"
#include "gui/Texture.hpp"
#include <time.h>

#include "lincity/engglobs.h"

#include <map>

class GameView : public Component
{
public:
    GameView(Component* parent, XmlReader& reader);
    ~GameView();
    
    void draw(Painter& painter);
    void resize(float width, float height );
    void event(const Event& event);

    //Show City Tile(x/y) by centering the screen 
    void show( const int x, const int y );
    void show( const Vector2 pos );

    //Set Zoomlevel to 100 Percent
    void resetZoom();
    //increase Zoomlevel 
    void zoomIn();
    //decrease Zoomlevel 
    void zoomOut();
private:
    const void recenter(const Vector2& pos);
    const Vector2 getScreenPoint(const Vector2& tile);
    const Vector2 getTile(const Vector2& point);
    const void drawTile(Painter& painter, const Vector2& tile);
    const void loadTextures();
    void setZoom(const int newzoom);
    Texture* readTexture(const std::string& filename);
    void readCityTexture( int textureType, const std::string& filename );

    static const int defaultTileWidth = 128;
    static const int defaultTileHeight = 64;
    static const int defaultZoom = 1000;

    int tileWidth, tileHeight, zoom; 
    //a virtual screen containing the whole city
    int virtualScreenWidth, virtualScreenHeight;

    //upper left corner of the viewport on virtual screen
    Vector2 viewport;
    
    //std::map<int, Texture*> cityTextures;
    Texture* cityTextures[ NUM_OF_TYPES ];
    Texture* blankTexture;
    int cityTextureX[ NUM_OF_TYPES ];
    int cityTextureY[ NUM_OF_TYPES ];
    
    Vector2 tileUnderMouse;
    bool mouseInGameView;
};

GameView* getGameView();

#endif

