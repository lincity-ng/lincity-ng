#ifndef __GAMEVIEW_HPP__
#define __GAMEVIEW_HPP__

#include "gui/Component.hpp"
#include "gui/XmlReader.hpp"
#include "gui/Vector2.hpp"
#include "gui/Texture.hpp"

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
 
private:
    const void recenter(Vector2 pos);
    const Vector2 getScreenPoint(Vector2 tile);
    const Vector2 getTile(Vector2 point);
    const void drawTile( Painter& painter, Vector2 tile );
    const void loadTextures();
    Texture* readTexture(const std::string& filename);
    void click(const Vector2 &pos);
    void setZoom(const int newzoom);

    static const int defaultTileWidth = 128;
    static const int defaultTileHeight = 64;

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
};

#endif

