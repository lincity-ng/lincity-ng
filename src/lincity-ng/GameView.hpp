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

    static const int tileWidth = 128;
    static const int tileHeight = 64;

    //a virtual screen containing the whole city
    static const int virtualScreenWidth = tileWidth * WORLD_SIDE_LEN;
    static const int virtualScreenHeight = tileHeight * WORLD_SIDE_LEN;

    //upper left corner of the viewport on virtual screen
    Vector2 viewport;
    
    //std::map<int, Texture*> cityTextures;
    Texture* cityTextures[ NUM_OF_TYPES ];
    Texture* blankTexture;
};

#endif

