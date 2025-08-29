/*
Copyright (C) 2024 David Bears <dbear4q@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef __WINDOW_MANAGER_HPP__
#define __WINDOW_MANAGER_HPP__

#include <stddef.h>       // for NULL
#include <memory>         // for unique_ptr
#include <vector>         // for vector

#include "Child.hpp"      // for Child
#include "Component.hpp"  // for Component
#include "Vector2.hpp"    // for Vector2

class Window;
namespace xmlpp {
class TextReader;
}  // namespace xmlpp

class WindowManager : public Component
{
public:
  WindowManager();
  virtual ~WindowManager();

  void parse(xmlpp::TextReader& reader);

  void resize(float width, float height) override;
  void event(const Event& event) override;
  bool opaque(const Vector2& pos) const override;

  void addWindow(std::unique_ptr<Window>&& window);
  void removeWindow(Window *window);

private:

  enum Edge : int {
    NONE = 0,
    N = 1 << 0,
    S = 1 << 1,
    W = 1 << 2,
    E = 1 << 3,
    NW = N | W,
    NE = N | E,
    SW = S | W,
    SE = S | E,
    NS = N | S,
    WE = W | E,
    NSW = N | S | W,
    NSE = N | S | E,
    NWE = N | W | E,
    SWE = S | W | E,
    NSWE = N | S | W | E,
  };
  Edge edgeAt(const Child &child, Vector2 pos) const;
  bool dragging = false;
  Window *dragWindow = NULL;
  Edge dragEdge = Edge::NONE;
  Vector2 dragOffset;
  bool hasMoved = false;
  static const int grabDist = 1;

  void addWindowInternal(std::unique_ptr<Window>&& window);
  void removeWindowInternal(Window *window);
  std::vector<Window *> removeQueue;
  std::vector<std::unique_ptr<Window>> addQueue;
  bool childLock = false;
  void lockChilds();
  void unlockChilds();
};

#endif


/** @file gui/Desktop.hpp */
