#ifndef __GRADIENT_HPP__
#define __GRADIENT_HPP__

#include "Component.hpp"
#include <memory>
#include <stdint.h>
#include "Color.hpp"

class XmlReader;
class Texture;

class Gradient : public Component
{
public:
    Gradient();
    virtual ~Gradient();

    void parse(XmlReader& reader);
    void resize(float width, float height);
    void draw(Painter& painter);

private:
    void draw_horizontal_line(SDL_Surface* surface, int x1, int y1, int x2,
                       uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void draw_vertical_line(SDL_Surface* surface, int x1, int y1, int y2,
                       uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    
    std::auto_ptr<Texture> texture;
    Color from, to;
    enum Direction {
        LEFT_RIGHT,
        TOP_BOTTOM
    };
    Direction direction;
};

#endif

