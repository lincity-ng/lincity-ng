#ifndef __RECTANGLE_H__
#define __RECTANGLE_H__

#include <assert.h>
#include "Vector2.hpp"

/** This class represents a rectangle.
 * (Implementation Note) We're using upper left and lower right point instead of
 * upper left and width/height here, because that makes the collision dectection
 * a little bit more efficient.
 */
class Rect2D
{
public:
  Rect2D()
  { }

  Rect2D(const Vector2& np1, const Vector2& np2)
    : p1(np1), p2(np2)
  {
  }

  Rect2D(float x1, float y1, float x2, float y2)
    : p1(x1, y1), p2(x2, y2)
  {
    assert(p1.x <= p2.x && p1.y <= p2.y);
  }

  float getWidth() const
  { return p2.x - p1.x; }

  float getHeight() const
  { return p2.y - p1.y; }

  Vector2 getMiddle() const
  { return Vector2((p1.x+p2.x)/2, (p1.y+p2.y)/2); }

  void setPos(const Vector2& v)
  {
    move(v-p1);
  }

  void setHeight(float height)
  {
    p2.y = p1.y + height;
  }
  void setWidth(float width)
  {
    p2.x = p1.x + width;
  }
  void setSize(float width, float height)
  {
    setWidth(width);
    setHeight(height);
  }                                         

  void move(const Vector2& v)
  {
    p1 += v;
    p2 += v;
  }

  bool inside(const Vector2& v) const
  {
    return v.x >= p1.x && v.y >= p1.y && v.x < p2.x && v.y < p2.y;
  }
  bool overlap(const Rect2D& other) const
  {
      if(p1.x >= other.p2.x || other.p1.x >= p2.x)
          return false;
      if(p1.y >= other.p2.y || other.p1.y >= p2.y)
          return false;

      return true;
  }

  void join(const Rect2D& other);

  /* leave these 2 vars public to safe the headaches of set/get functions
   * for such simple things :)
   */
  /// upper left edge
  Vector2 p1;
  /// lower right edge
  Vector2 p2;
};

#endif
